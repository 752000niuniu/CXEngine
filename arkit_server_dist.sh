rm -rf arkit_server

mkdir -p arkit_server/.vscode
cp launch.json arkit_server/.vscode

mkdir -p arkit_server/bin
cp bin/Release/cxluax.exe arkit_server/bin/

mkdir -p arkit_server/res
cp -r res/font arkit_server/res/


mkdir -p arkit_server/scripts
cp -r scripts/arkit arkit_server/scripts/
cp -r scripts/share arkit_server/scripts/

cp imgui.ini arkit_server/

touch arkit_server/launcher.bat
echo 'start bin/cxluax.exe  --cwd=%cd%/ --script_path=scripts/arkit/' > arkit_server/launcher.bat
