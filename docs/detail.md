---
layout: none
---
#### What is the __Windows File Watcher {brand-plugin-type}__?
A {brand-name} {brand-plugin-type} you can configure to watch file locations on the windows systems and report suspicous activity

#### What are the features?

- __Lazy On Access Scanning__ - Monitor file access and automatically submit files for analysis
- __Out of Box Defaults__ - Comes with default API keys and configuation so it works out of the box
- __Configurable Watch Directories__ - Configure which directories to watch for activity
- __Infected File Deletion__ - Delete infected files
- __{brand-name} Portal__ - With a registered API key you can access all the core Secplugs features via the portal.

#### How does it work?

Once installed, the {brand-plugin-type} is notified by Windows when a file is created in or moved to the configured directories. 
The file is sent to {brand-name} for a score based scan. If this scan detects this file to be malicious, the file is removed from the filesystem.
This tools uses a very minimal configuration that is just enough. The list of directories to monitor is the only mandatory configuration.


#### How do I get started?

Download the tool from the link below, unzip to a temporary location and follow the instructions in the Install-HowTo.txt file.


[secplugs-windows-filewatcher.zip](https://packages.secplugs.com/secplugs-windows-filewatcher.zip)
