// Copyright (C) 1999-2005 Open Source Telecom Corporation.
// Copyright (C) 2006-2010 David Sugar, Tycho Softworks.
//
// This file is part of GNU uCommon C++.
//
// GNU uCommon C++ is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published 
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GNU uCommon C++ is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with GNU uCommon C++.  If not, see <http://www.gnu.org/licenses/>.

#include <config.h>
#include <ucommon/protocols.h>
#include <ucommon/string.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

using namespace UCOMMON_NAMESPACE;

char *MemoryProtocol::dup(const char *str)
{
    if(!str)
        return NULL;
	size_t len = strlen(str) + 1;
    char *mem = static_cast<char *>(alloc(len));
    String::set(mem, len, str);
    return mem;
}

void *MemoryProtocol::dup(void *obj, size_t size)
{
	assert(obj != NULL);
	assert(size > 0);

    char *mem = static_cast<char *>(alloc(size));
	memcpy(mem, obj, size);
    return mem;
}

void *MemoryProtocol::zalloc(size_t size)
{
	void *mem = alloc(size);

	if(mem)
		memset(mem, 0, size);

	return mem;
}

MemoryRedirect::MemoryRedirect(MemoryProtocol *protocol)
{
	target = protocol;
}

void *MemoryRedirect::_alloc(size_t size)
{
	// a null redirect uses the heap...
	if(!target)
		return malloc(size);

	return target->_alloc(size);
}

BufferProtocol::BufferProtocol()
{
	end = true;
	eol = "\r\n";
	input = output = buffer = NULL;
}

BufferProtocol::BufferProtocol(size_t size, type_t mode)
{
	end = true;
	eol = "\r\n";
	input = output = buffer = NULL;
	allocate(size, mode);
}

BufferProtocol::~BufferProtocol()
{
	release();
}

void BufferProtocol::release(void)
{
	if(buffer) {
		flush();
		free(buffer);
		input = output = buffer = NULL;
		end = true;
	}
}

void BufferProtocol::allocate(size_t size, type_t mode)
{
	release();
	_clear();

	if(!size)
		return;

	switch(mode) {
	case BUF_RDWR:
		input = buffer = (char *)malloc(size * 2);
		if(buffer)
			output = buffer + size;
		break;
	case BUF_RD:
		input = buffer = (char *)malloc(size);
		break;
	case BUF_WR:
		output = buffer = (char *)malloc(size);
		break;
	}

	bufpos = insize = outsize = 0;
	bufsize = size;

	if(buffer)
		end = false;
}

bool BufferProtocol::_blocking(void)
{
	return false;
}

size_t BufferProtocol::get(char *address, size_t size)
{
	size_t count = 0;

	if(!input || !address)
		return 0;

	while(count < size) {
		if(bufpos == insize) {
			if(end)
				return count;

			insize = _pull(input, bufsize);
			bufpos = 0;
			if(insize == 0)
				end = true;
			else if(insize < bufsize && !_blocking())
				end = true;

			if(!insize)
				return count;
		}
		address[count++] = input[bufpos++];
	}
	return count;
}

int BufferProtocol::_getch(void)
{
	if(!input)
		return EOF;

	if(bufpos == insize) {
		if(end)
			return EOF;

		insize = _pull(input, bufsize);
		bufpos = 0;
		if(insize == 0)
			end = true;
		else if(insize < bufsize && !_blocking())
			end = true;

		if(!insize)
			return EOF;
	}

	return input[bufpos++];
}

size_t BufferProtocol::put(const char *address, size_t size)
{
	size_t count = 0;

	if(!output || !address)
		return 0;

	if(!size)
		size = strlen(address);

	while(count < size) {
		if(outsize == bufsize) {
			outsize = 0;
			if(_push(output, bufsize) < bufsize) {
				output = NULL;
				end = true;		// marks a disconnection...
				return count;
			}
		}
		
		output[outsize++] = address[count++];
	}

	return count;
}

int BufferProtocol::_putch(int ch)
{
	if(!output)
		return EOF;

	if(outsize == bufsize) {
		outsize = 0;
		if(_push(output, bufsize) < bufsize) {
			output = NULL;
			end = true;		// marks a disconnection...
			return EOF;
		}
	}
	
	output[outsize++] = ch;
	return ch;
}

size_t BufferProtocol::printf(const char *format, ...)
{
	va_list args;
	int result;
	size_t count;

	if(!flush() || !output || !format)
		return 0;

	va_start(args, format);
	result = vsnprintf(output, bufsize, format, args);
	va_end(args);
	if(result < 1)
		return 0;

	if((size_t)result > bufsize)
		result = bufsize;

	count = _push(output, result);
	if(count < (size_t)result) {
		output = NULL;
		end = true;
	}

	return count;
}

void BufferProtocol::purge(void)
{
	outsize = insize = bufpos = 0;
}

bool BufferProtocol::_flush(void)
{
	if(!output)
		return false;

	if(!outsize)
		return true;
	
	if(_push(output, outsize) == outsize) {
		outsize = 0;
		return true;
	}
	output = NULL;
	end = true;
	return false;
}

char *BufferProtocol::gather(size_t size)
{
	if(!input || size > bufsize)
		return NULL;

	if(size + bufpos > insize) {
		if(end)
			return NULL;
	
		size_t adjust = outsize - bufpos;
		memmove(input, input + bufpos, adjust);
		insize = adjust +  _pull(input, bufsize - adjust);
		bufpos = 0;

		if(insize < bufsize)
			end = true;
	}

	if(size + bufpos <= insize) {
		char *bp = input + bufpos;
		bufpos += size;
		return bp;
	}

	return NULL;	
}

char *BufferProtocol::request(size_t size)
{
	if(!output || size > bufsize)
		return NULL;

	if(size + outsize > bufsize)
		flush();

	size_t offset = outsize;
	outsize += size;
	return output + offset;
}	

size_t BufferProtocol::putline(const char *string)
{
	size_t count = 0;

	if(string)
		count += put(string);

	if(eol)
		count += put(eol);

	return count;
}

size_t BufferProtocol::getline(string& s)
{
	size_t result = getline(s.c_mem(), s.size() + 1);
	String::fix(s);
	return result;
}

size_t BufferProtocol::getline(char *string, size_t size)
{
	size_t count = 0;
	unsigned eolp = 0;
	const char *eols = eol;
	bool eof = false;
	
	if(!eols)
		eols="\0";

	if(string)
		string[0] = 0;

	if(!input || !string)
		return 0;

	while(count < size - 1) {
		int ch = BufferProtocol::_getch();
		if(ch == EOF) {
			eolp = 0;
			eof = true;
			break;
		}

		string[count++] = ch;

		if(ch == eols[eolp]) {
			++eolp;
			if(eols[eolp] == 0)
				break;
		}
		else
			eolp = 0;	

		// special case for \r\n can also be just eol as \n 
		if(eq(eol, "\r\n") && ch == '\n') {
			++eolp;
			break;
		}
	}
	count -= eolp;
	string[count] = 0;
	if(!eof)
		++count;
	return count;
}

void BufferProtocol::reset(void)
{
	insize = bufpos = 0;
}

bool BufferProtocol::eof(void)
{
	if(!input)
		return true;

	if(end && bufpos == insize)
		return true;

	return false;
}
	
bool BufferProtocol::_pending(void)
{
	if(!input)
		return false;

	if(!bufpos)
		return false;

	return true;
}

void LockingProtocol::_lock(void)
{
}

void LockingProtocol::_unlock(void)
{
}
	