Provisioning a Windows EC2 instance as a Jenkins CI server

 1. Install chocolatey
 2. Manually install jdk6 32/64-bit
 3. Manually install Strawberry Perl
 4. choco install -y svn git python2 7zip.commandline doxygen.install graphviz wix35 gow nasm netfx-4.5.1-devpack netfx-4.5.2-devpack visualstudio2012wdx sandcastle html-help-workshop jdk7
 5. choco install -y visualstudio2015community -packageParameters "--AdminFile C:\Path\To\AdminDeployment.xml"
 6. Enable .net Framework 3.5 feature
 7. Ensure PATH is something similar to: %SystemRoot%\system32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SYSTEMROOT%\System32\WindowsPowerShell\v1.0\;C:\Program Files\Amazon\cfn-bootstrap\;C:\ProgramData\chocolatey\bin;C:\Program Files (x86)\Subversion\bin;C:\Program Files\Git\cmd;C:\tools\python2;C:\Program Files\doxygen\bin;C:\Program Files (x86)\Gow\bin;C:\Program Files\Microsoft SQL Server\110\Tools\Binn\;C:\Program Files\Java\jdk1.7.0_79\bin;%USERPROFILE%\.dnx\bin;C:\Program Files\Microsoft DNX\Dnvm\;C:\Program Files (x86)\Windows Kits\8.1\Windows Performance Toolkit\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\Strawberry\c\bin;C:\Strawberry\perl\site\bin;C:\Strawberry\perl\bin;C:\tools\python2\Scripts
 8. Copy jenkins.war to an accessible drive and start it
 9. Install jenkins as a service (so it automatically starts up on subsequent startups)
 10. Expose port 8080 in windows firewall (and allow inbound 8080 on your EC2 security group) to access the Jenkins CI web frontend outside.