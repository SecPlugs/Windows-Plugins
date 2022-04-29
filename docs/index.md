# Windows File Watcher
Designed to run on Microsoft Windows and scan file activity in configurable locations.

The tool is open source so you can modify as you wish.
Also see [Power Automate Listing](/plugin-list/plugin-secplugs-windows-plugins-win-file-watcher)

## How does it work?

Once installed, the {brand-plugin-type} is notified by Windows when a file is created in or moved to the configured directories. 
The file is sent to {brand-name} for a score based scan. If this scan detects this file to be malicious, the file is removed from the filesystem.
This tools uses a very minimal configuration that is just enough. The list of directories to monitor is the only mandatory configuration.


## How do I get started?

Download the tool from the link below, unzip to a temporary location and follow the instructions in the Install-HowTo.txt file.


[secplugs-windows-filewatcher.zip](https://packages.secplugs.com/secplugs-windows-filewatcher.zip)