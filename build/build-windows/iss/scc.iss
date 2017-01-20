[Setup]
AppVerName=Simple Chat Client
AppVersion=3.0.0
VersionInfoVersion=3.0.0
OutputBaseFilename=scc-3.0.0.0

AppName=Simple Chat Client
AppPublisher=Simple Chat Client
AppPublisherURL=https://simplechatclient.github.io
AppSupportURL=https://github.com/simplechatclient/simplechatclient/issues
AppUpdatesURL=https://simplechatclient.github.io/download

DefaultDirName={pf32}\SimpleChatClient
DefaultGroupName=Simple Chat Client
UninstallDisplayIcon={app}\scc.ico

WizardSmallImageFile=scc.bmp

Compression=lzma2
SolidCompression=yes

PrivilegesRequired=admin
CloseApplications=force
RestartApplications=no

DisableReadyMemo=yes
DisableReadyPage=yes
DisableWelcomePage=yes
DisableStartupPrompt=yes
DisableDirPage=yes
DisableProgramGroupPage=yes
DisableFinishedPage=yes

ShowLanguageDialog=auto

[Languages]
Name: en; MessagesFile: "compiler:Default.isl"
Name: de; MessagesFile: "compiler:Languages\German.isl"
Name: pl; MessagesFile: "compiler:Languages\Polish.isl"

[Files]
Source: "release\*"; DestDir: "{app}"; Flags: recursesubdirs ignoreversion
Source: "license.txt"; DestDir: "{app}"
Source: "scc.ico"; DestDir: "{app}"
Source: "vcredist_x86.exe"; DestDir: {tmp};  Flags: deleteafterinstall

[Icons]
Name: "{group}\Simple Chat Client"; Filename: "{app}\scc.exe"
Name: "{commondesktop}\Simple Chat Client"; Filename: "{app}\scc.exe"

[Run]
Filename: "{tmp}\vcredist_x86.exe"; Parameters: "/q /norestart"; StatusMsg: Installing Visual C++ Redistributable Package...
Filename: "{app}\scc.exe"; Description: "{cm:LaunchProgram,Simple Chat Client}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: filesandordirs; Name: "{userdocs}\scc"
Type: dirifempty; Name: "{userdocs}\scc"

; variables to code
#define MyAppName "scc"
#define MyAppExeName "scc.exe"

[Code]
function InitializeSetup(): Boolean;
  var ErrorCode: Integer;
begin
  ShellExec('open','taskkill.exe','/IM {#MyAppExeName} /T /F','',SW_HIDE,ewNoWait,ErrorCode);
  ShellExec('open','tskill.exe',' {#MyAppName}','',SW_HIDE,ewNoWait,ErrorCode);
  result := True;
end;

function InitializeUninstall(): Boolean;
  var ErrorCode: Integer;
begin
  ShellExec('open','taskkill.exe','/IM {#MyAppExeName} /T /F','',SW_HIDE,ewNoWait,ErrorCode);
  ShellExec('open','tskill.exe',' {#MyAppName}','',SW_HIDE,ewNoWait,ErrorCode);
  result := True;
end;
