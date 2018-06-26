# GMS: Grid-based Motion Statistics for Fast, Ultra-robust Feature Correspondence  

C++ implementation of the [Grid-based Motion Statistics for Feature Correspondance](http://jwbian.net/Papers/GMS_CVPR17.pdf), by Bian et al.  
  
The Grid-Based Motion Statistics is a method for improving the quality of feature matches, through motion-smoothness encapsulation. Based on ORB feature matches, the smoothness constraint is expressed as a statistical likelihood of the per-cell matching frequency. Results show both real-time and very robust feature correspondence.  
<div align="center">
<table style="width:100%; margin-left: auto;" align="center">
  <tr>
    <th><div align="center"><img width="250" src ="https://raw.githubusercontent.com/germain-hug/GMS-Feature-Correspondence/master/images/orb.jpg" /></div></th>
    <th><div align="center"><img width="250" src ="https://raw.githubusercontent.com/germain-hug/GMS-Feature-Correspondence/master/images/gms.jpg" /></div></th>
    <th><div align="center"><img width="250" src ="https://github.com/germain-hug/GMS-Feature-Correspondence/blob/master/images/orb.gif?raw=true" /></div></th>
    <th><div align="center"><img width="250" src ="https://github.com/germain-hug/GMS-Feature-Correspondence/blob/master/images/gms.gif?raw=true" /></div></th> 
  </tr>
  <tr>
    <th> ORB feature points (2000) </th>
    <th> After GMS </th>
    <th> ORB Feature points (2000) </th>
    <th> After GMS </th>
  </tr>
</table>
</div>


## Running  
This implementation requires [OpenCV 2.4.13.2](https://opencv.org/releases.html)  
```bash  
mkdir build && make    
./GMS <path_to_image_1> <path_to_image_2>
```  

