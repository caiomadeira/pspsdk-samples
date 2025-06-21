docker run -it --rm \
  -u $(id -u):$(id -g) \
  -v ~/Desktop/pspsdk-samples:/home/dev/projects \
  -w /home/dev/projects \
  pspdev/pspdev
