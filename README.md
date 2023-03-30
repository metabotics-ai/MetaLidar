# MetaLidar
A plugin that simulates LiDAR sensor data built on top of Unreal Engine.

# Supported Platform
Linux

# Supported Sensor
|Vender|Model|
|------|:---:|
|Velodyne|VLP-16|
||VLP-32C|

# System Requirements
|Element|Requirement|
|:------:|---|
|**OS**|Ubuntu 22.04|
|**CPU**|Intel Core i9, X-series or higher |
||AMD Ryzen 9, Threadripper or higher|
|**Cores**|16 or higher|
|**RAM**|32GB or higher|
|**GPU**|RTX 3070 or higher|
|**UE5**|v5.1.x|

# Quickstart Guide
The MetaLidar plugin should be installed in the 'Plugins' folder inside your Unreal Engine 5 project. Also, you should install the [**UDP-Unreal**](https://github.com/getnamo/UDP-Unreal/releases/tag/v2.1.0) plugin, which is a convenience *ActorComponent* UDP wrapper for Unreal Engine.
An example setup using this plugin can be found at [MetaLidarSampleUE5](https://github.com/metabotics-ai/MetaLidarSampleUE5) project.