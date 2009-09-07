; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=Euclide
AppVerName=Euclide 0.90
AppPublisher=�tienne Dupuis
AppPublisherURL=http://lestourtereaux.free.fr/euclide/
AppSupportURL=http://lestourtereaux.free.fr/euclide/
AppUpdatesURL=http://lestourtereaux.free.fr/euclide/
DefaultDirName={pf}\Euclide
DefaultGroupName=Euclide
AlwaysCreateUninstallIcon=true

AppVersion=0.90
; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

[Tasks]

[Files]
Source: C:\Mes documents\Disque ZIP\Programmes\Euclide\Version 0.90\Euclide.exe; DestDir: {app}; CopyMode: alwaysoverwrite
Source: C:\Mes documents\Disque ZIP\Programmes\Euclide\Version 0.90\Output.txt; DestDir: {app}; CopyMode: alwaysoverwrite
Source: C:\Mes documents\Disque ZIP\Programmes\Euclide\Version 0.90\Input.txt; DestDir: {app}; CopyMode: alwaysoverwrite
Source: C:\Mes documents\Disque ZIP\Programmes\Euclide\Version 0.90\Help File\Euclide.html; DestDir: {app}; CopyMode: alwaysoverwrite
Source: C:\Mes documents\Disque ZIP\Programmes\Euclide\Version 0.90\Language Files\Nederlands.txt; DestDir: {app}; CopyMode: alwaysoverwrite
Source: C:\Mes documents\Disque ZIP\Programmes\Euclide\Version 0.90\Language Files\Fran�ais.txt; DestDir: {app}; CopyMode: alwaysoverwrite
Source: C:\Mes documents\Disque ZIP\Programmes\Euclide\Version 0.90\Language Files\English.txt; DestDir: {app}; CopyMode: alwaysoverwrite
Source: C:\Mes documents\Disque ZIP\Programmes\Euclide\Version 0.90\Language Files\Deutsch.txt; DestDir: {app}; CopyMode: alwaysoverwrite
Source: Set Language.bat; DestDir: {app}
Source: Euclide.txt; DestDir: {app}

[Icons]
Name: {group}\Euclide; Filename: {app}\Euclide.exe; Parameters: Input.txt; WorkingDir: {app}; IconIndex: 0
Name: {group}\Input File; Filename: {app}\Input.txt; IconIndex: 0
Name: {group}\Output File; Filename: {app}\Output.txt; IconIndex: 0
Name: {group}\Help; Filename: {app}\Euclide.html; IconIndex: 0
Name: {group}\Language Files\Deutsch; Filename: {app}\Set Language.bat; Parameters: Deutsch.txt; WorkingDir: {app}; IconIndex: 0; Flags: runminimized closeonexit
Name: {group}\Language Files\English; Filename: {app}\Set Language.bat; Parameters: English.txt; WorkingDir: {app}; IconIndex: 0; Flags: runminimized closeonexit
Name: {group}\Language Files\Fran�ais; Filename: {app}\Set Language.bat; Parameters: Fran�ais.txt; WorkingDir: {app}; IconIndex: 0; Flags: runminimized closeonexit
Name: {group}\Language Files\Nederlands; Filename: {app}\Set Language.bat; Parameters: Nederlands.txt; WorkingDir: {app}; IconIndex: 0; Flags: runminimized closeonexit
Name: {group}\DOS Prompt; Filename: {win}\Command.com; WorkingDir: {app}; IconIndex: 0

[Run]
Filename: {app}\Euclide.html; Description: Read Help File; Flags: nowait postinstall skipifsilent shellexec; WorkingDir: {app}

[_ISTool]
EnableISX=false

