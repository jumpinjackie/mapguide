//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: SyntaxManager.hpp,v 1.13 2005/04/05 16:44:03 bostic Exp $
//

#ifndef __SYNTAXMANAGER_HPP
#define	__SYNTAXMANAGER_HPP

#include <vector>
#include "Syntax.hpp"

namespace DbXml
{

class SyntaxManager
{
public:
	static SyntaxManager *getInstance();

	SyntaxManager();
	~SyntaxManager();

	void registerSyntax(const Syntax &syntax);
	const Syntax *getSyntax(const std::string &name) const;
	const Syntax *getNextSyntax(int &i) const;
	const Syntax *getSyntax(Syntax::Type type) const;
	size_t size() const;

public:
	static void initSyntaxManager(void);
	static void uninitSyntaxManager(void);
private:
	int findSyntax(const std::string &name) const;
	typedef std::vector<const Syntax *> SyntaxVector;
	SyntaxVector sv_;
};

}

#endif
