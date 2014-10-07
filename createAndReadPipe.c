#include <stdio.h>
#include <windows.h>

#define PASSTHROUGH_BUFFER_SIZE 4096

#define PIPE_PATH_PREFIX "\\\\.\\pipe\\"

#define EXIT_CODE_USAGE 1
#define EXIT_CODE_CREATE_FAILED 2
#define EXIT_CODE_CONNECT_FAILED 3
#define EXIT_CODE_READ_FAILED 4
#define EXIT_CODE_MALLOC_FAILED 255

static char * programName;

#define PERROR(...) { fprintf(stderr, "%s: ", programName); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); fflush(stderr); }
#define PINFO(...) { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); fflush(stderr);  }

static char * mallocAndCat(const char * left, const char * right)
{
	size_t left_len = strlen(left);
	size_t right_len = strlen(right);

	char * buffer = malloc(left_len + right_len + 1);
	char * cursor = buffer;

	if(buffer) {
		memcpy(cursor, left, left_len);
		cursor += left_len;
		memcpy(cursor, right, right_len);
		cursor += right_len;
		cursor[0] = 0;
	}

	return buffer;
}



#define ERROR_CODE_STRING_BUFFER_SIZE 12

static BOOL createPipe(HANDLE * p_pipe, const char* pipePath)
{
	*p_pipe = CreateNamedPipe(
		pipePath,
		PIPE_ACCESS_INBOUND,
		PIPE_TYPE_BYTE,
		1, // max instances
		0, // out buffer size
		0, // in buffer size
		0, // default timeout
		NULL // security attributes
		);

	if(*p_pipe == INVALID_HANDLE_VALUE) {
		int err = GetLastError();

		switch(err) {
			case ERROR_ACCESS_DENIED:
				PERROR("Could not open pipe %s: access was denied.", pipePath);
				break;
			default:
				PERROR("Could not open pipe %s; error code was 0x%08x.", pipePath, err);
				break;
		}
		return FALSE;
	}

	PINFO("Successfully created %s for read.", pipePath);

	return TRUE;
}

static BOOL waitForClient(HANDLE pipe)
{
	if(ConnectNamedPipe(pipe, NULL)) {
		return TRUE;
	}

	int err = GetLastError();

	if(err == ERROR_PIPE_CONNECTED) {
		// Failed only because already connected
		return TRUE;
	}

	PERROR("Error waiting for client; error code was 0x%08x.", err);

	return FALSE;
}

static BOOL runPassthrough(HANDLE pipe)
{
	char buffer[PASSTHROUGH_BUFFER_SIZE];

	DWORD readLen;
	BOOL result;

	while(TRUE)
	{
		result = ReadFile(
			pipe,
			buffer,
			PASSTHROUGH_BUFFER_SIZE * sizeof(char),
			&readLen,
			NULL);

		if(result) {
			fwrite(buffer, sizeof(char), readLen, stdout);
			fflush(stdout);
		}
		else {
			int err = GetLastError();
			if(err == ERROR_BROKEN_PIPE) {
				// Client disconnected
				// We treat this as EOF rather than an error
				PINFO("Reached end of input (client has disconnected).");
				fclose(stdout);
				return TRUE;
			}
			else {
				PERROR("Error reading from client; error code was 0x%08x.", err);
				return FALSE;
			}
		}
	}
}



int main(int argc, char** argv)
{
	programName = argv[0];

	if(argc < 2) {
		PINFO("Usage: %s PIPENAME", programName);
		PINFO("Creates a named pipe at %sPIPENAME and reads it to stdout.", PIPE_PATH_PREFIX);
		return EXIT_CODE_USAGE;
	}

	HANDLE pipe;
	char* pipePath = mallocAndCat(PIPE_PATH_PREFIX, argv[1]);

	if(pipePath == NULL) {
		PERROR("Out of memory.");
		return EXIT_CODE_MALLOC_FAILED;
	}

	if(!createPipe(&pipe, pipePath)) {
		return EXIT_CODE_CREATE_FAILED;
	}

	free(pipePath);

	PINFO("Waiting for client...");

	BOOL isConnected = waitForClient(pipe);

	int exitCode;

	if(isConnected) {
		if(runPassthrough(pipe)) {
			exitCode = 0;
		}
		else {
			exitCode = EXIT_CODE_READ_FAILED;
		}
	}
	else
	{
		exitCode = EXIT_CODE_CONNECT_FAILED;
	}

	PINFO("Closing pipe.");
	CloseHandle(pipe);

	return exitCode;
} 


