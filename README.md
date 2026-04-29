```
sudo apt update && sudo apt install -y python3.12-venv cmake libsdl2-2.0-0 libsdl2-dev build-essential && git clone https://github.com/emscripten-core/emsdk && cd emsdk && ./emsdk install latest && ./emsdk activate latest && source ./emsdk_env.sh

. emsdk/emsdk_env.sh && git clone --recursive https://github.com/OneLevelStudio/LVGL_WEB && cd LVGL_WEB && mkdir cmbuild && cd cmbuild && emcmake cmake .. && emmake make -j4

. emsdk/emsdk_env.sh && cd LVGL_WEB/cmbuild && emmake make -j4

. emsdk/emsdk_env.sh && rm -rf LVGL_WEB/cmbuild && cd LVGL_WEB && mkdir cmbuild && cd cmbuild && emcmake cmake .. && emmake make -j4
```