s!public \(.*long cPtr, \)!/* package */ \1!
s!public \(class dbxml_java\)!/* package */ \1!
s!public final \(.*0(\)!/* package */ final \1!
s!public void delete()!& /* no exception */!
s!\(public .*(.*)\) {!\1 throws XmlException {!
s!toString() throws XmlException {!toString() {!
s!swigValue() throws XmlException {!swigValue() {!
s!swigToEnum() throws XmlException {!swigToEnum() {!
s!get_version_major() throws XmlException {!get_version_major() {!
s!get_version_minor() throws XmlException {!get_version_minor() {!
s!get_version_patch() throws XmlException {!get_version_patch() {!
s!get_version_string() throws XmlException {!get_version_string() {!
/^  protected XmlManager() {$/,/^  }$/d
s!dynamic_cast<SwigDirector_XmlResolver \*>!(SwigDirector_XmlResolver \*)!g
s!dynamic_cast<SwigDirector_XmlInputStream \*>!(SwigDirector_XmlInputStream \*)!g
s!bool result ;!bool result = false;!
