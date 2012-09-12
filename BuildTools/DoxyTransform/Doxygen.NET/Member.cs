/*
 * Doxygen.NET - .NET object wrappers for Doxygen
 * Copyright 2009 - Ra-Software AS
 * This code is licensed under the LGPL version 3.
 * 
 * Authors: 
 * Thomas Hansen (thomas@ra-ajax.org)
 * Kariem Ali (kariem@ra-ajax.org)
 * 
 */

using System;
using System.Collections.Generic;
using System.Text;

namespace Doxygen.NET
{
    public class Member: IDocItem
    {
        #region IDocItem Members

        public virtual string ID { get; protected internal set; }
        public virtual string Kind { get; protected internal set; }
        public virtual string FullName { get; protected internal set; }
        public virtual string Summary { get; protected internal set; }
        public virtual string Description { get; protected internal set; }

        #endregion

        public string Name { get; protected internal set; }
        public virtual string AccessModifier { get; protected internal set; }
        public virtual string ReturnType { get; protected internal set; }
        public Type Parent { get; protected internal set; }
    }
}
