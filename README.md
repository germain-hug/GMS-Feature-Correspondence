# GMS: Grid-based Motion Statistics for Fast, Ultra-robust Feature Correspondence  

Unofficial C++ implementation of the [Grid-based Motion Statistics for Feature Correspondance](http://jwbian.net/Papers/GMS_CVPR17.pdf), by Bian et al.  

<div align="center">
<table style="width:100%; margin-left: auto;" align="center">
  <tr>
    <th><div align="center"><img width="250" src ="https://raw.githubusercontent.com/germain-hug/GMS-Feature-Correspondence/master/images/orb.jpg" /></div></th>
    <th><div align="center"><img width="250" src ="https://raw.githubusercontent.com/germain-hug/GMS-Feature-Correspondence/master/images/gms.jpg" /></div></th>
    <th><div align="center"><img width="250" src ="https://github.com/germain-hug/GMS-Feature-Correspondence/blob/master/images/orb.gif?raw=true" /></div></th>
    <th><div align="center"><img width="250" src ="https://github.com/germain-hug/GMS-Feature-Correspondence/blob/master/images/gms.gif?raw=true" /></div></th> 
  </tr>
  <tr>
    <th> Raw ORB feature points </th>
    <th> Feature points after GMS </th>
    <th> Raw ORB feature points </th>
    <th> Feature points after GMS </th>
  </tr>
</table>
</div>


## Running  
This implementation requires [OpenCV 2.4.13.2](https://opencv.org/releases.html)  
```bash  
mkdir build && make    
./GMS <path_to_image_1> <path_to_image_2>
```  

