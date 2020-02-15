mkdir -p dist/bin
cp bin/Release/SimpleServer.exe dist/bin/
cp bin/Release/SimpleEngine.exe dist/bin/
cp bin/Release/cxluax.exe dist/bin/

mkdir -p dist/res
cp -r res/font dist/res/
cp -r res/icon dist/res/
cp -r res/shader dist/res/
cp -r res/storage dist/res/
cp -r res/tables dist/res/

# mkdir -p dist/res/data
# cp -r res/data/scene   dist/res/data
# cp res/data/addon.wdf   dist/res/data
# cp res/data/atom.wdf    dist/res/data
# cp res/data/chat.wdf    dist/res/data
# cp res/data/firework.wdf    dist/res/data
# cp res/data/goods.wdf   dist/res/data
# cp res/data/item.wd1    dist/res/data
# cp res/data/item.wdf    dist/res/data
# cp res/data/magic.wd1   dist/res/data
# cp res/data/magic.wdf   dist/res/data
# cp res/data/mapani.wdf  dist/res/data
# cp res/data/mhimage.wd1 dist/res/data
# cp res/data/mhimage.wdf dist/res/data
# cp res/data/mhxiu.wdf   dist/res/data
# cp res/data/misc.wdf    dist/res/data
# cp res/data/music.wdf   dist/res/data
# cp res/data/mypal.wdf   dist/res/data
# cp res/data/scene.wdf   dist/res/data
# cp res/data/shape.wd1   dist/res/data
# cp res/data/shape.wd2   dist/res/data
# cp res/data/shape.wd3   dist/res/data
# cp res/data/shape.wd4   dist/res/data
# cp res/data/shape.wd5   dist/res/data
# cp res/data/shape.wd6   dist/res/data
# cp res/data/shape.wd7   dist/res/data
# cp res/data/shape.wd8   dist/res/data
# cp res/data/shape.wd9   dist/res/data
# cp res/data/shape.wda   dist/res/data
# cp res/data/shape.wdf   dist/res/data
# cp res/data/smap.wdf    dist/res/data
# cp res/data/sound.wdf   dist/res/data
# cp res/data/stock.wdf   dist/res/data
# cp res/data/waddon.wdf  dist/res/data
# cp res/data/wzife.wd1   dist/res/data
# cp res/data/wzife.wd2   dist/res/data
# cp res/data/wzife.wd3   dist/res/data
# cp res/data/wzife.wdf   dist/res/data
# cp res/data/wzimage.wdf dist/res/data

mkdir -p dist/scripts
cp -r scripts/client dist/scripts/
cp -r scripts/combat dist/scripts/
cp -r scripts/server dist/scripts/
cp -r scripts/share dist/scripts/
cp -r scripts/generator dist/scripts/
cp -r scripts/launcher dist/scripts/

cp imgui.ini dist/

touch dist/launcher.bat
echo 'start bin/cxluax.exe  --cwd=%cd%/ --script_path=scripts/launcher/' > dist/launcher.bat

