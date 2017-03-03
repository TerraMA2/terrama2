# Frequently asked questions

### - The web application didn't start automatically

If you have the **pm2** in your system when the **TerraMA2** is installed some configurations are not made to avoid conflict. If you want to add the **pm2** to your system startup use:

```
sudo env PATH=$PATH:/usr/local/bin pm2 startup
```

After that you need to add the **TerraMA2**  to the **pm2**:

```
cd /opt/terrama2/4.x.x/webapp
sudo pm2 start npm --name webapp -- start
cd /opt/terrama2/4.x.x/webmonitor
sudo pm2 start npm --name webmonitor -- start
sudo pm2 save
```
