rm -rf face_export

mkdir -p face_export/.vscode
cp launch.json face_export/.vscode

mkdir -p face_export/bin
cp bin/Release/cxluax.exe face_export/bin/
cp bin/Animation.exe face_export/bin/

mkdir -p face_export/res
cp -r res/font face_export/res/


mkdir -p face_export/scripts
cp -r scripts/face_export face_export/scripts/
cp -r scripts/share face_export/scripts/

cp imgui.ini face_export/

touch face_export/launcher.bat
echo 'start bin/cxluax.exe  --cwd=%cd%/ --script_path=scripts/face_export/' > face_export/launcher.bat
