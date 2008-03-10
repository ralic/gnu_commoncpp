// Copyright (C) 1999-2007 David Sugar, Tycho Softworks.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.

/**
 * Any ucommon streaming classes that are built from std::streamio facilities
 * and that support ANSI C++ stream operators.
 * @file ucommon/stream.h
 */

#if defined(OLD_STDCPP) || defined(NEW_STDCPP)
#ifndef	_UCOMMON_STREAM_H_
#define	_UCOMMON_STREAM_H_

#ifndef	_UCOMMON_THREAD_H_
#include <ucommon/thread.h>
#endif

#ifndef	_UCOMMON_SOCKET_H_
#include <ucommon/socket.h>
#endif

#ifndef	_UCOMMON_FSYS_H_
#include <ucommon/fsys.h>
#endif

#include <iostream>

NAMESPACE_UCOMMON

/**
 * Streamable tcp connection between client and server.  The tcp stream
 * class can represent a client connection to a server or an instance of
 * a service generated by a tcp listener.  As a stream class, data can 
 * be manipulated using the << and >> operators.
 *
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT tcpstream : protected std::streambuf, public std::iostream
{
private:
	__LOCAL void allocate(unsigned size);
	__LOCAL	void reset(void);

protected:
	socket_t so;
	timeout_t timeout;
	size_t bufsize;
	char *gbuf, *pbuf;

	/**
	 * Release the tcp stream and destroy the underlying socket.
	 */
	void release(void);

    /**
     * This streambuf method is used to load the input buffer
     * through the established tcp socket connection.
     *
     * @return char from get buffer, EOF if not connected.
     */
    int underflow();

    /**
     * This streambuf method is used for doing unbuffered reads
     * through the establish tcp socket connection when in interactive mode.
     * Also this method will handle proper use of buffers if not in
     * interative mode.
     *
     * @return char from tcp socket connection, EOF if not connected.
     */
    int uflow();

    /**
     * This streambuf method is used to write the output
     * buffer through the established tcp connection.
     *
     * @param ch char to push through.
     * @return char pushed through.
     */
    int overflow(int ch);

public:
	/**
	 * Copy constructor...
	 * @param copy for object.
	 */
	tcpstream(const tcpstream& copy);

	/**
	 * Create a stream from an existing tcp listener.
	 * @param listener to accept connection from.
	 * @param segsize for tcp segments and buffering.
	 * @param timeout for socket i/o operations.
	 */
	tcpstream(ListenSocket& listener, unsigned segsize = 536, timeout_t timeout = 0);

	/**
	 * Create an unconnected tcp stream object that is idle until opened.
	 * @param family of protocol to create.
	 * @param timeout for socket i/o operations.
	 */
	tcpstream(int family = PF_INET, timeout_t timeout = 0);

	/**
	 * A convenience constructor that creates a connected tcp stream directly
	 * from an address.  The socket is constructed to match the type of the
	 * the address family in the socket address that is passed.
	 * @param address of service to connect to.
	 * @param segsize for tcp segments and buffering.
	 * @param timeout for socket i/o operations.
	 */
	tcpstream(Socket::address& address, unsigned segsize = 536, timeout_t timeout = 0);

	/**
	 * Destroy a tcp stream.
	 */
	virtual ~tcpstream();

	/**
	 * See if stream connection is active.
	 * @return true if stream is active.
	 */
	inline operator bool()
		{return so != INVALID_SOCKET && bufsize > 0;};

	/**
	 * See if stream is disconnected.
	 * @return true if stream disconnected.
	 */
	inline bool operator!()
		{return so == INVALID_SOCKET || bufsize == 0;};

	/**
	 * Open a stream connection to a tcp service.
	 * @param address of service to access.
	 * @param buffering segment size to use.
	 */
	void open(Socket::address& address, unsigned mss = 536);

	/**
	 * Close an active stream connection.  This does not release the
	 * socket but is a disconnect.
	 */
	void close(void);

	/**
	 * Flush the stream input and output buffers, writes pending output.
	 * @return 0 on success, or error code.
	 */
	int sync(void);	
};

/**
 * Streamable tcp connection between client and server.  The tcp stream
 * class can represent a client connection to a server or an instance of
 * a service generated by a tcp listener.  As a stream class, data can 
 * be manipulated using the << and >> operators.
 *
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT pipestream : protected std::streambuf, public std::iostream
{
public:
	typedef enum {
		RDONLY,
		WRONLY,
		RDWR
	} access_t;

private:
	__LOCAL void allocate(size_t size, access_t mode);

protected:
	fsys_t rd, wr;
	pid_t pid;
	size_t bufsize;
	char *gbuf, *pbuf;

	/**
	 * Release the stream, detach/do not wait for the process.
	 */
	void release(void);

    /**
     * This streambuf method is used to load the input buffer
     * through the established pipe connection.
     *
     * @return char from get buffer, EOF if not connected.
     */
    int underflow();

    /**
     * This streambuf method is used for doing unbuffered reads
     * through the establish tcp socket connection when in interactive mode.
     * Also this method will handle proper use of buffers if not in
     * interative mode.
     *
     * @return char from pipe connection, EOF if not connected.
     */
    int uflow();

    /**
     * This streambuf method is used to write the output
     * buffer through the established pipe connection.
     *
     * @param ch char to push through.
     * @return char pushed through.
     */
    int overflow(int ch);

public:
	/**
	 * Create an unopened pipe stream.
	 */
	pipestream();

	/**
	 * Create child process and start pipe.
	 * @param command to pass.
	 * @param arguments of child process.
	 * @param access mode of pipe stream.
	 * @param size of buffer.
	 */
	pipestream(const char *command, char **arguments, access_t access, size_t size = 512);

	/**
	 * Destroy a pipe stream.
	 */
	virtual ~pipestream();

	/**
	 * See if stream connection is active.
	 * @return true if stream is active.
	 */
	inline operator bool()
		{return (bufsize > 0);};

	/**
	 * See if stream is disconnected.
	 * @return true if stream disconnected.
	 */
	inline bool operator!()
		{return bufsize == 0;};

	/**
	 * Open a stream connection to a tcp service.
	 * @param command to execute.
	 * @param arguments to pass.
	 * @param access mode of stream.
	 * @param buffering size to use.
	 */
	void open(const char *command, char **arguments, access_t access, size_t buffering = 512);

	/**
	 * Close an active stream connection.  This waits for the child to
	 * terminate.
	 */
	void close(void);

	/**
	 * Force terminate child and close.
	 */
	void terminate(void);

	/**
	 * Flush the stream input and output buffers, writes pending output.
	 * @return 0 on success, or error code.
	 */
	int sync(void);	
};

/**
 * Streamable tcp connection between client and server.  The tcp stream
 * class can represent a client connection to a server or an instance of
 * a service generated by a tcp listener.  As a stream class, data can 
 * be manipulated using the << and >> operators.
 *
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT filestream : protected std::streambuf, public std::iostream
{
public:
	typedef enum {
		RDONLY,
		WRONLY,
		RDWR
	} access_t;

private:
	__LOCAL void allocate(size_t size, fsys::access_t mode);

protected:
	fsys_t fd;
	fsys::access_t ac;
	size_t bufsize;
	char *gbuf, *pbuf;

    /**
     * This streambuf method is used to load the input buffer
     * through the established pipe connection.
     *
     * @return char from get buffer, EOF if not connected.
     */
    int underflow();

    /**
     * This streambuf method is used for doing unbuffered reads
     * through the establish tcp socket connection when in interactive mode.
     * Also this method will handle proper use of buffers if not in
     * interative mode.
     *
     * @return char from pipe connection, EOF if not connected.
     */
    int uflow();

    /**
     * This streambuf method is used to write the output
     * buffer through the established pipe connection.
     *
     * @param ch char to push through.
     * @return char pushed through.
     */
    int overflow(int ch);

public:
	/**
	 * Create an unopened pipe stream.
	 */
	filestream();

	/**
	 * Create duplicate stream.
	 */
	filestream(const filestream& copy);

	/**
	 * Create file stream.
	 */
	filestream(const char *path, fsys::access_t access, unsigned mode, size_t bufsize);

	/**
	 * Open file stream.
	 */
	filestream(const char *path, fsys::access_t access, size_t bufsize);

	/**
	 * Destroy a file stream.
	 */
	virtual ~filestream();

	/**
	 * See if stream connection is active.
	 * @return true if stream is active.
	 */
	inline operator bool()
		{return (bufsize > 0);};

	/**
	 * See if stream is disconnected.
	 * @return true if stream disconnected.
	 */
	inline bool operator!()
		{return bufsize == 0;};

	/**
	 * Open a stream connection to a tcp service.
	 */
	void open(const char *filename, fsys::access_t access, size_t buffering = 512);

	/**
	 * Create a stream connection to a tcp service.
	 */
	void create(const char *filename, fsys::access_t access, unsigned mode, size_t buffering = 512);

	/**
	 * Close an active stream connection.
	 */
	void close(void);

	/**
	 * Seek position.
	 */
	void seek(fsys::offset_t offset);

	/**
	 * Flush the stream input and output buffers, writes pending output.
	 * @return 0 on success, or error code.
	 */
	int sync(void);	

	/**
	 * Get error flag from last i/o operation.
	 * @return last error.
     */
	inline int error(void)
		{return fd.getError();};
};

END_NAMESPACE

#endif
#endif
