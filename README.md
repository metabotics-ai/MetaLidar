# MetaLidar
A plugin that simulates LiDAR sensor data built on top of Unreal Engine.

# Supported Platform
Linux

# Supported Sensor
Currently, following sensor models are supported:
- **Velodyne**
  - VLP-16
  - PUCK-Lite
  - PUCK-HiRes
  - VLP-32C
  - *Limitations*
    - Position packet is not implemented yet.
    - Laser return mode : only *'Strongest'* is available


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
The MetaLidar plugin should be installed in the 'Plugins' folder inside your Unreal Engine 5 project. Also, you should install the [**UDP-Unreal**](https://github.com/getnamo/UDP-Unreal/releases/tag/v2.1.0) plugin, which is a convenience *ActorComponent* UDP wrapper for Unreal Engine. After launching the Unreal Editor, make sure that both plugins are enabled.

![MetaLidar_plugin](Resources/MetaLidar_plugin.png)
![UDP-Unreal_plugin](Resources/UDP-Unreal_plugin.png)


An example project using this plugin can be found at [MetaLidarSampleUE5](https://github.com/metabotics-ai/MetaLidarSampleUE5).


# How to use
## LiDAR intensity
Most LiDAR sensors output not only the distance to the object, but also intensity information, meaning the strength of the reflected signal. The intensity depends on a number of factors, including the material and color of the object's surface, but in general, darker colors tend to have smaller values and lighter colors tend to have larger values. To set an object's intensity property using a *PhysicalMaterial*, you can do the following steps:
- Create PhysicalMaterial named 'PM_Reflectivity_XXX'
- Here, XXX represents a value from 0 to 255, where lower numbers mean less reflectivity and higher numbers mean more reflectivity.
- Apply the created PhysicalMaterial to the static mesh's physical material property.

![PM_Configure](Resources/PM_configure.gif)

## Transparent objects
Since the laser output from LiDAR passes through transparent objects, it is crucial to be reflected this for an accurate simulation. To configure an object's transparency, you can do the following steps:
- Select the object you want to give the transparency to.
- Change collision preset(Physics > Collosion) to ***'Custom'***
- Change *'visibility'* of Collision Responses to ***'Overlap'***

![Transparency](Resources/Transparency_configure.gif)