# prismatic-em-DockerFile_baseimage2

* yutoml/prismatic_baseとしてDocker HUBにある
* gpuを使う場合`docker run -it --gpus all yutoml/prismatic_base:1.0`とするのを忘れずに

## prismatic-em_Dockerfile_gpu-gui

* 以下のコマンドをwsl上で実行する
* docker run -it --gpus all -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY
