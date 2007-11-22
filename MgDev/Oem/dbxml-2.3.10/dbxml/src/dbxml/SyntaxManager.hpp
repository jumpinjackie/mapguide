//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: SyntaxManager.hpp,v 1.16 2006/10/30 17:45:53 bostic Exp $
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
	typedef std::vector<const Syntax *> SyntaxVector;
	SyntaxVector sv_;
};

}

#endif
