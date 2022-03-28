#include "query.hh"

#ifdef _WIN32

#include <windows.h>
#include <atlstr.h>
std::string ExecShellCommand(const char* cmd)
{
    //https://stackoverflow.com/a/35658917/1786598

    CStringA strResult;
    HANDLE   hPipeRead, hPipeWrite;

    SECURITY_ATTRIBUTES saAttr  = {sizeof(SECURITY_ATTRIBUTES)};
    saAttr.bInheritHandle       = TRUE;  // Pipe handles are inherited by child process.
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe to get results from child's stdout.
    if (!CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0)) return strResult;

    STARTUPINFOW si = {sizeof(STARTUPINFOW)};
    si.dwFlags      = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.hStdOutput   = hPipeWrite;
    si.hStdError    = hPipeWrite;
    si.wShowWindow  = SW_HIDE;  // Prevents cmd window from flashing.
                                // Requires STARTF_USESHOWWINDOW in dwFlags.

    PROCESS_INFORMATION pi = {0};

    const size_t WCHARBUF = 1024;
    wchar_t      cmdWide[WCHARBUF];
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cmd, -1, cmdWide, WCHARBUF);

    BOOL fSuccess = CreateProcessW(NULL, (LPWSTR)cmdWide, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    if (!fSuccess) {
        CloseHandle(hPipeWrite);
        CloseHandle(hPipeRead);
        return strResult;
    }

    bool bProcessEnded = false;
    for (; !bProcessEnded;) {
        // Give some timeslice (50 ms), so we won't waste 100% CPU.
        bProcessEnded = WaitForSingleObject(pi.hProcess, 50) == WAIT_OBJECT_0;

        // Even if process exited - we continue reading, if
        // there is some data available over pipe.
        for (;;) {
            char  buf[1024];
            DWORD dwRead  = 0;
            DWORD dwAvail = 0;

            if (!::PeekNamedPipe(hPipeRead, NULL, 0, NULL, &dwAvail, NULL)) break;

            if (!dwAvail)  // No data available, return
                break;

            if (!::ReadFile(hPipeRead, buf, min(sizeof(buf) - 1, dwAvail), &dwRead, NULL) || !dwRead)
                // Error, the child process might ended
                break;

            buf[dwRead] = 0;
            strResult += buf;
        }
    }  // for

    CloseHandle(hPipeWrite);
    CloseHandle(hPipeRead);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return strResult;
}

#else

std::string ExecShellCommand(const char* cmd)
{
    std::array<char, 128>                    buffer;
    std::string                              result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) { throw std::runtime_error("popen() failed!"); }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) { result += buffer.data(); }
    return result;
}

#endif


void GetMachineProperties()
{
    std::vector<std::string> v;
    cout << "host information: " << endl;

    auto cpuinfo = ExecShellCommand(  //
        std::string("cat /proc/cpuinfo "
                    "| grep \"model name\" "
                    "| head -n 1 "
                    "| awk -F': ' '{print $NF}'")
            .c_str());
    cout << "  cpu model\t" << cpuinfo;

    auto meminfo = ExecShellCommand(  //
        std::string("cat /proc/meminfo"
                    "| grep \"MemTotal\" "
                    "| awk -F' ' '{print $2\" \"$3}'")
            .c_str());

    cout << "  memory size\t" << meminfo;

    auto endianness = ExecShellCommand(  //
        std::string("lscpu "
                    "| grep Endian "
                    "| awk -F'  ' '{print $NF}'")
            .c_str());

    cout << "  byte order\t" << endianness;
    printf("\n");
}

void GetDeviceProperty()
{
    int         num_dev  = 0;
    cudaError_t error_id = cudaGetDeviceCount(&num_dev);

    if (error_id != cudaSuccess) {
        printf("cudaGetDeviceCount returned %d\n-> %s\n", static_cast<int>(error_id), cudaGetErrorString(error_id));
        exit(EXIT_FAILURE);
    }
    if (num_dev == 0) { printf("NO CUDA device detected.\n"); }
    int dev, driver_ver = 0, runtime_ver = 0;

    for (dev = 0; dev < num_dev; ++dev) {
        cudaSetDevice(dev);
        cudaDeviceProp dev_prop;
        cudaGetDeviceProperties(&dev_prop, dev);
        printf("device #%d, %s: \n", dev, dev_prop.name);

        cudaDriverGetVersion(&driver_ver);
        cudaRuntimeGetVersion(&runtime_ver);
        printf(
            "  driver/runtime\t%d.%d/%d.%d\n", driver_ver / 1000, (driver_ver % 100) / 10, runtime_ver / 1000,
            (runtime_ver % 100) / 10);
        printf("  compute capability:\t%d.%d\n", dev_prop.major, dev_prop.minor);
        printf("  global memory:\t%.0f MiB\n", static_cast<float>(dev_prop.totalGlobalMem / 1048576.0f));
        printf("  constant memory:\t%zu bytes\n", dev_prop.totalConstMem);
        printf("  shared mem per block:\t%zu bytes\n", dev_prop.sharedMemPerBlock);
        printf("  shared mem per SM:\t%zu bytes\n", dev_prop.sharedMemPerMultiprocessor);
        printf("  registers per block:\t%d\n", dev_prop.regsPerBlock);
    }
    printf("\n");
}
