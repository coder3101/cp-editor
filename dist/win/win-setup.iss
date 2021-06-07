; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

; MyRootOut = directory where you ran windeployqt
; MyAppVersion = App Version
; MyProjectRoot = Source root
; Arch = x86 or x64
; GccDir = (optional) Path to GCC

#include "environment.iss"

#define MyAppName "CP Editor"
#define MyAppPublisher "Ashar Khan <coder3101>"
#define MyAppExeName "cpeditor.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{01D77782-3A40-44F0-BD36-59626998F4EE}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL="https://cpeditor.org"
AppSupportURL="https://github.com/cpeditor/cpeditor/discussions"
AppUpdatesURL="https://github.com/cpeditor/cpeditor/releases"
DefaultDirName={autopf}\cpeditor
DisableProgramGroupPage=yes
UsedUserAreasWarning=no
LicenseFile={#MyProjectRoot}\LICENSE
PrivilegesRequiredOverridesAllowed=dialog
OutputDir={#MyProjectRoot}
OutputBaseFilename=cpeditor-setup
SetupIconFile={#MyProjectRoot}\assets\icon.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern
ChangesEnvironment=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 6.1; Check: not IsAdminInstallMode

[Files]
Source: "{#MyOutRoot}\cpeditor.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyOutRoot}\bearer\*"; DestDir: "{app}\bearer\"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#MyOutRoot}\iconengines\*"; DestDir: "{app}\iconengines\"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#MyOutRoot}\imageformats\*"; DestDir: "{app}\imageformats\"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#MyOutRoot}\platforms\*"; DestDir: "{app}\platforms\"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#MyOutRoot}\styles\*"; DestDir: "{app}\styles\"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#MyOutRoot}\*.dll"; DestDir: "{app}"; Flags: ignoreversion

; VC++ redistributable runtime. Extracted by VC2019RedistNeedsInstall(), if needed.
Source: "{#MyOutRoot}\Redist\vc_redist.{#Arch}.exe"; DestDir: {tmp}; Flags: dontcopy

#ifdef GccDir
Source: "{#MyOutRoot}\{#GccDir}\*"; DestDir: "{app}\{#GccDir}\"; Flags: ignoreversion recursesubdirs createallsubdirs
#endif

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
Filename: "{tmp}\vc_redist.{#Arch}.exe"; StatusMsg: "Installing VC2019 redist..."; Parameters: "/quiet /norestart"; Check: VC2019RedistNeedsInstall ; Flags: waituntilterminated

[Code]
function VC2019RedistNeedsInstall: Boolean;
var
  Version: String;
  ExpectedVersion: String;
begin
  if (RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\{#Arch}', 'Version', Version)) then
  begin
    ExpectedVersion := 'v{#VC_REDIST_VERSION}.03'
    Log('VC Redist Version check : found ' + Version);
    Log('VC Redist Version check : expected ' + ExpectedVersion);
    Result := (CompareStr(Version, ExpectedVersion)<0);
  end
  else
  begin
    // Not even an old version installed
    Result := True;
  end;
  if (Result) then
  begin
    ExtractTemporaryFile('vc_redist.{#Arch}.exe');
  end;
end;

#ifdef GccDir
procedure CurStepChanged(CurStep: TSetupStep);
begin
    if CurStep = ssPostInstall then
    EnvAddPath(ExpandConstant('{app}') +'\{#GccDir}\bin');
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
    if CurUninstallStep = usPostUninstall then
    EnvRemovePath(ExpandConstant('{app}') +'\{#GccDir}\bin');
end;
#endif

[Registry]
Root: HKCR; Subkey: ".cpp\OpenWithProgIds\{#MyAppName}"; Flags: uninsdeletekey
Root: HKCR; Subkey: ".c++\OpenWithProgIds\{#MyAppName}"; Flags: uninsdeletekey
Root: HKCR; Subkey: ".cxx\OpenWithProgIds\{#MyAppName}"; Flags: uninsdeletekey
Root: HKCR; Subkey: ".cc\OpenWithProgIds\{#MyAppName}"; Flags: uninsdeletekey
Root: HKCR; Subkey: ".c\OpenWithProgIds\{#MyAppName}"; Flags: uninsdeletekey
Root: HKCR; Subkey: ".h\OpenWithProgIds\{#MyAppName}"; Flags: uninsdeletekey
Root: HKCR; Subkey: ".hpp\OpenWithProgIds\{#MyAppName}"; Flags: uninsdeletekey
Root: HKCR; Subkey: ".py\OpenWithProgIds\{#MyAppName}"; Flags: uninsdeletekey
Root: HKCR; Subkey: ".py3\OpenWithProgIds\{#MyAppName}"; Flags: uninsdeletekey
Root: HKCR; Subkey: ".java\OpenWithProgIds\{#MyAppName}"; Flags: uninsdeletekey
Root: HKCR; Subkey: "{#MyAppName}\DefaultIcon"; ValueType: string; ValueName: "{app}\{#MyAppExeName},1"; Flags: uninsdeletekey
Root: HKCR; Subkey: "{#MyAppName}\shell\open\command"; ValueType: string; ValueName: "";  ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Flags: uninsdeletekey