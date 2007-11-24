if [ -e lib/jx_combined.css ]
then
    rm lib/jx_combined.css
fi
if [ -e lib/jx_combined.js ]
then
    rm lib/jx_combined.js
fi
if [ -e lib/jx_compressed.js ]
then
    rm lib/jx_compressed.js
fi
if [ ! -e docs/NaturalDocs ]
then
    mkdir docs/NaturalDocs
fi
../NaturalDocs/NaturalDocs -i ./ -o html ./docs -p ./docs/NaturalDocs -xi ./scriptaculous -xi ./lib -r
cat license.js > lib/jx_combined.js
cat license.js > lib/jx_compressed.js
cat scriptaculous/prototype.js scriptaculous/builder.js scriptaculous/effects.js scriptaculous/dragdrop.js scriptaculous/controls.js scriptaculous/slider.js >> lib/jx_combined.js
cat jxcore.js >> lib/jx_combined.js
find . -mindepth 2 -name "jx[^_]*.js" -exec cat "{}" >> lib/jx_combined.js \;
cat jxstyles.js >> lib/jx_combined.js
jsmin < lib/jx_combined.js >> lib/jx_compressed.js
cat reset.css >> combined.css
find . -mindepth 2 -maxdepth 2 -name "*.css" -exec cat "{}" >> combined.css \;
mv combined.css lib/jx_combined.css