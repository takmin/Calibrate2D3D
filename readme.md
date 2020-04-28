# Calibrate2D3D

Estimate external parameters (relative 3D pose) between 2D images and 3D point clouds.

Two inputs are needed:
1. 2D-3D point pairs obtained by "Plot2D3DPair"
    - https://github.com/takmin/Plot2D3DPair
2. Internal camera parameters of 2D Camera obtained by "CameraCalibration"
    - https://github.com/takmin/CameraCalibration


An example of this command is as follow:

```
Calibrate2D3D -K=internal_param.yml -P=points_pair.yml -O=external_param.yml
```

-K option requires internal camera parameter file obtained by "CameraCalibration" (step 2)
-P option requires point pairs between 2D image and 3D point cloud obtained by "Plot2D3DPair" (step 1)
-O option requires output file name which describes external camera parameter from 3D sensor coordinates to camera coordinates.