//
// File_VX.h
//
// Library: Foundation
// Package: Filesystem
// Module:  File
//
// Definition of the FileImpl class for VxWorks.
//
// Copyright (c) 2004-2011, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#ifndef Foundation_File_VX_INCLUDED
#define Foundation_File_VX_INCLUDED


#include "Poco/Foundation.h"
#include "Poco/Timestamp.h"

namespace Poco {


class FileImpl
{
protected:

	enum Options {
		OPT_FAIL_ON_OVERWRITE_IMPL = 0x01
	};

	using FileSizeImpl = UInt64;

	FileImpl();
	FileImpl(const std::string& path);
	virtual ~FileImpl();
	void swapImpl(FileImpl& file);
	void setPathImpl(const std::string& path);
	const std::string& getPathImpl() const;
	std::string getExecutablePathImpl() const;
	bool existsImpl() const;
	bool canReadImpl() const;
	bool canWriteImpl() const;
	bool canExecuteImpl(const std::string& absolutePath) const;
	bool isFileImpl() const;
	bool isDirectoryImpl() const;
	bool isLinkImpl() const;
	bool isDeviceImpl() const;
	bool isHiddenImpl() const;
	Timestamp createdImpl() const;
	Timestamp getLastModifiedImpl() const;
	void setLastModifiedImpl(const Timestamp& ts);
	FileSizeImpl getSizeImpl() const;
	void setSizeImpl(FileSizeImpl size);
	void setWriteableImpl(bool flag = true);
	void setExecutableImpl(bool flag = true);
	void copyToImpl(const std::string& path, int options = 0) const;
	void renameToImpl(const std::string& path, int options = 0);
	void linkToImpl(const std::string& path, int type) const;
	void removeImpl();
	bool createFileImpl(bool createDirectories = false);
	bool createDirectoryImpl();
	FileSizeImpl totalSpaceImpl() const;
	FileSizeImpl usableSpaceImpl() const;
	FileSizeImpl freeSpaceImpl() const;
	static void handleLastErrorImpl(const std::string& path);

private:
	std::string _path;

	friend class DirectoryIteratorImpl;
};


//
// inlines
//
inline const std::string& FileImpl::getPathImpl() const
{
	return _path;
}


} // namespace Poco


#endif // Foundation_File_VX_INCLUDED
