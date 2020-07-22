rm -rf example
mkdir -p example/bin
cp bin/Debug/CXEngine.exe example/bin/
cp bin/Debug/cxluax.exe example/bin/

mkdir -p example/res
cp -r res/font example/res/
cp -r res/icon example/res/
cp -r res/shader example/res/
cp -r res/tables example/res/

mkdir -p example/data
# cp -r data/scene   example/data
# cp data/addon.wdf   example/data
# cp data/atom.wdf    example/data
# cp data/chat.wdf    example/data
# cp data/firework.wdf    example/data
# cp data/goods.wdf   example/data
# cp data/item.wd1    example/data
# cp data/item.wdf    example/data
# cp data/magic.wd1   example/data
# cp data/magic.wdf   example/data
# cp data/mapani.wdf  example/data
# cp data/mhimage.wd1 example/data
# cp data/mhimage.wdf example/data
# cp data/mhxiu.wdf   example/data
# cp data/misc.wdf    example/data
# cp data/music.wdf   example/data
# cp data/mypal.wdf   example/data
# cp data/scene.wdf   example/data
# cp data/shape.wd1   example/data
# cp data/shape.wd2   example/data
# cp data/shape.wd3   example/data
# cp data/shape.wd4   example/data
# cp data/shape.wd5   example/data
# cp data/shape.wd6   example/data
# cp data/shape.wd7   example/data
# cp data/shape.wd8   example/data
# cp data/shape.wd9   example/data
# cp data/shape.wda   example/data
# cp data/shape.wdf   example/data
# cp data/smap.wdf    example/data
# cp data/sound.wdf   example/data
# cp data/stock.wdf   example/data
# cp data/waddon.wdf  example/data
# cp data/wzife.wd1   example/data
# cp data/wzife.wd2   example/data
# cp data/wzife.wd3   example/data
# cp data/wzife.wdf   example/data
# cp data/wzimage.wdf example/data

mkdir -p example/scripts
cp -r scripts/share example/scripts/
cp -r scripts/launcher example/scripts/
cp -r scripts/examples example/scripts/

cp imgui.ini example/

touch example/launcher.bat
echo 'start bin/cxluax.exe  --cwd=%cd%/ --script_path=scripts/launcher/' > example/launcher.bat

