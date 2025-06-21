sudo apt update
sudo apt install docker.io -y
sudo usermod -aG docker $USER
newgrp docker

echo "downloading pspdev image...."
sudo docker pull pspdev/pspdev
sudo docker run -it --rm pspdev/pspdev
sudo docker run -it --rm -v ~/Desktop/pspsdk-samples:/root/projects pspdev/pspdev
echo "go to root/projects/ in container to start making applications"
sudo chown -R $USER:$USER ~/Desktop/pspsdk-samples
