// Copyright © Clinton Ingram and Contributors.  Licensed under the MIT License.
// IIS Compression Scheme DLL export function definitions.  See <httpcompression.h>

#include "brotli.h"

// Create a new compression context, called at the start of each response to be compressed.
HRESULT WINAPI CreateCompression(OUT PVOID *context, IN ULONG reserved)
{
	// Passing null pointers here tells the Brotli encoder to allocate its own memory.
	*context = BrotliEncoderCreateInstance(NULL, NULL, NULL);
	return *context ? S_OK : E_FAIL;
}

// Destroy compression context, called at the end of each compressed response.
VOID WINAPI DestroyCompression(IN PVOID context)
{
	// Any memory the encoder allocated is freed here.
	BrotliEncoderDestroyInstance((BrotliEncoderState*)context);
}

// Compress data, called in a loop until full response is processed.
HRESULT WINAPI Compress(
	IN  OUT PVOID           context,            // compression context
	IN      CONST BYTE *    input_buffer,       // input buffer
	IN      LONG            input_buffer_size,  // size of input buffer
	IN      PBYTE           output_buffer,      // output buffer
	IN      LONG            output_buffer_size, // size of output buffer
	OUT     PLONG           input_used,         // amount of input buffer used
	OUT     PLONG           output_used,        // amount of output buffer used
	IN      INT             compression_level   // compression level (0..11)
)
{
	if (compression_level < BROTLI_MIN_QUALITY || compression_level > BROTLI_MAX_QUALITY)
		return E_INVALIDARG;

	BrotliEncoderState* encoderState = (BrotliEncoderState*)context;

	// This should succeed on the first call and fail once any data has been processed.
	// Since IIS won't change compression level between calls, we can ignore the return.
	BrotliEncoderSetParameter(encoderState, BROTLI_PARAM_QUALITY, compression_level);

	*input_used = 0;
	*output_used = 0;

	size_t size_in = input_buffer_size;
	size_t size_out = output_buffer_size;

	// Compress input data.  If input is empty, we're at the end of the stream, so finish up.
	BrotliEncoderOperation op = input_buffer_size ? BROTLI_OPERATION_PROCESS : BROTLI_OPERATION_FINISH;
	if (!BrotliEncoderCompressStream(encoderState, op, &size_in, &input_buffer, &size_out, &output_buffer, NULL))
		return E_FAIL;

	*input_used = input_buffer_size - (LONG)size_in;
	*output_used = output_buffer_size - (LONG)size_out;

	// Return S_OK to continue looping, S_FALSE to complete the response.
	return input_buffer_size || !BrotliEncoderIsFinished(encoderState) ? S_OK : S_FALSE;
}
