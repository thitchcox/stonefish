.. _sensors:

=======
Sensors
=======

A rich set of sensor simulations is available in the *Stonefish* library, including the ones specific for the marine robotics. The implemented sensors can be divided into three groups: the joint sensors, the link sensors and the vision sensors. Each sensor type is described below to understand its operation and the way to include it in the simulation scenario. Most of the sensors include appropriate noise models which can be optionally enabled. 

.. warning:: 

    Sensors can only be created in connection with a definition of a :ref:`robot <robots>`, because they have to be attached to a robot's joint or link.

.. note::

    In the following sections, description of each specific sensor implementation is accompanied with an example of sensor instantiation through the XML syntax and the C++ code. It is assumed that the XML snippets are located inside the definition of a robot. In case of C++ code, it is assumed that an object ``sf::Robot* robot = new sf::Robot(...);`` was created before the sensor definition. 

Joint sensors
=============

The joint sensors are attached to the robot's joints and measure their internal states. All of them share the following properties:

1) **Name:** unique string

2) **Rate:** sensor update frequency [Hz] (optional)

3) **Type:** type of the sensor

4) **History length**: the size of the measurement buffer

5) **Joint name**: the name of the robot joint that the sensor is attached to

.. code-block:: xml
    
    <sensor name="{1}" rate="{2}" type="{3}">
       <!-- specific definitions here -->
       <history samples="{4}"/>
       <joint name="{5}"/>
    </sensor>

Rotary encoder
--------------

The rotary encoder measures the rotation angle of a specified joint. It does not have any specific properties.

.. code-block:: xml

    <sensor name="Encoder" rate="10.0" type="encoder">
        <history samples="100"/>
        <joint name="Joint1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/scalar/RotaryEncoder.h>
    sf::RotaryEncoder* encoder = new sf::RotaryEncoder("Encoder", 10.0, 100);
    robot->AddJointSensor(encoder, "Joint1");

Torque (1-axis)
---------------

The torque sensor measures the torque excerted on a specified joint. The measurement range and the standard deviation of the measured torque can be optionally defined.

.. code-block:: xml

    <sensor name="Torque" rate="100.0" type="torque">
        <range torque="10.0"/>
        <noise torque="0.05"/>
        <history samples="100"/>
        <joint name="Joint1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/scalar/Torque.h>
    sf::Torque* torque = new sf::Torque("Torque", 100.0, 100);
    torque->setRange(10.0);
    torque->setNoise(0.05);
    robot->AddJointSensor(torque, "Joint1");

Force-torque (6-axis)
---------------------

The force-torque sensor is a 6-axis sensor located in a specified joint. It measures force and torque in all three directions of a Cartesian reference frame, attached to the child link of the joint. The measurement range for each of the sensor channels and the standard deviation of measurements can be optionally defined.

.. code-block:: xml

    <sensor name="FT" rate="100.0" type="forcetorque">
        <range force="10.0 10.0 100.0" torque="1.0 1.0 2.0"/>
        <noise force="0.5" torque="0.05"/>
        <origin xyz="0.0 0.0 0.0" rpy="0.0 0.0 0.0"/>
        <history samples="1"/>
        <joint name="Joint1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/scalar/ForceTorque.h>
    sf::ForceTorque* ft = new sf::ForceTorque("FT", sf::I4(), 100.0, 1);
    ft->setRange(sf::Vector3(10.0, 10.0, 100.0), sf::Vector3(1.0, 1.0, 2.0));
    ft->setNoise(0.5, 0.05);
    robot->AddJointSensor(ft, "Joint1");

.. _link-sensors:

Link sensors
============

The link sensors measure motion related or environment related quantities. They are attached to the robot's links. All of them share the following properties:

1) **Name:** unique string

2) **Rate:** sensor update frequency [Hz] (optional)

3) **Type:** type of the sensor

4) **History length**: the size of the measurement buffer

5) **Origin:** the transformation from the link frame to the sensor frame

6) **Link name**: the name of the robot link that the sensor is attached to

.. code-block:: xml
    
    <sensor name="{1}" rate="{2}" type="{3}">
       <!-- specific definitions here -->
       <history samples="{4}"/>
       <origin xyz="{5a}" rpy="{5b}"/>
       <link name="{6}"/>
    </sensor>

IMU
---

The inertial measurement unit (IMU) measures the orientation and the angular velocities of the link. The angular velocity measurement range and the standard deviation of angle and angular velocity measurements can be optionally defined.

.. code-block:: xml

    <sensor name="IMU" rate="10.0" type="imu">
        <range angular_velocity="0.5"/>
        <noise angle="0.1" angular_velocity="0.05"/>
        <history samples="1"/>
        <origin xyz="0.1 0.0 0.0" rpy="0.0 0.0 0.0"/>
        <link name="Link1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/scalar/IMU.h>
    sf::IMU* imu = new sf::IMU("IMU", 10.0, 1);
    imu->setRange(0.5);
    imu->setNoise(0.1, 0.05);
    robot->AddLinkSensor(imu, "Link1", sf::Transform(sf::Quaternion(0.0, 0.0, 0.0), sf::Vector3(0.1, 0.0, 0.0));

Odometry
--------

The odometry sensor is a virtual sensor which can be used to obtain the navigation ground truth or emulate navigation system with errors.
It measures position, linear velocities, orientation and angular velocities. Standard deviation for each of the quantities can be optionally specified.

.. code-block:: xml

    <sensor name="Odometry" rate="10.0" type="odometry">
        <noise position="0.05" velocity="0.01" angle="0.1" angular_velocity="0.05"/>
        <history samples="1"/>
        <origin xyz="0.0 0.0 0.0" rpy="0.0 0.0 0.0"/>
        <link name="Link1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/scalar/Odometry.h>
    sf::Odometry* odom = new sf::Odometry("Odometry", 10.0, 1);
    odom->setNoise(0.05, 0.01, 0.1, 0.05);
    robot->AddLinkSensor(odom, "Link1", sf::I4());

GPS
---

The global positioning system (GPS) sensor measures the position of the link in the world frame and converts it into the geographic coordinates and altitude. This sensor works only when above the water level. Optionally, it is possible to define the standard deviation of the position error in the NED frame. 

.. code-block:: xml

    <sensor name="GPS" rate="1.0" type="gps">
        <noise ned_position="0.5"/>
        <history samples="1"/>
        <origin xyz="0.0 0.0 0.0" rpy="0.0 0.0 0.0"/>
        <link name="Link1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/scalar/GPS.h>
    sf::GPS* gps = new sf::GPS("GPS", 1.0, 1);
    gps->setNoise(0.5);
    robot->AddLinkSensor(gps, "Link1", sf::I4());

Compass
-------

The compass is measuring the heading of the robot. Optionally it is possible to define standard deviation of the measurement.

.. code-block:: xml

    <sensor name="Compass" rate="1.0" type="compass">
        <noise heading="0.1"/>
        <history samples="1"/>
        <origin xyz="0.0 0.0 0.0" rpy="0.0 0.0 0.0"/>
        <link name="Link1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/scalar/Compass.h>
    sf::Compass* comp = new sf::Compass("Compass", 1.0, 1);
    comp->setNoise(0.1);
    robot->AddLinkSensor(comp, "Link1", sf::I4());

Pressure
--------

The pressure sensor measures the gauge pressure underwater. Pressure range as well as standard deviation of the measurement can be defined. 

.. code-block:: xml

    <sensor name="Pressure" rate="1.0" type="pressure">
        <range pressure="10000.0"/>
        <noise pressure="0.1"/>
        <history samples="1"/>
        <origin xyz="0.0 0.0 0.0" rpy="0.0 0.0 0.0"/>
        <link name="Link1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/scalar/Pressure.h>
    sf::Pressure* press = new sf::Pressure("Pressure", 1.0, 1);
    press->setRange(10000.0);
    press->setNoise(0.1);
    robot->AddLinkSensor(press, "Link1", sf::I4());

Doppler velocity log (DVL)
--------------------------

The Doppler velocity log (DVL) is a classic marine craft sensor, used for measuring vehicle velocity as well as water velocity. The current implementation of DVL in the Stonefish library is using four acoustic beams to determine the altitude above terrain. The shortest distance is reported. Moreover, it provides robot velocity along all three Cartesian axes. The velocity is calculated based on the simulation of motion rather than the Doppler effect, which may be improved in future. It is possible to specify sensor operating range in terms of the altitude limits as well as the maximum measured velocity. Noise can be added to the measurements as well.

.. code-block:: xml

    <sensor name="DVL" rate="10.0" type="dvl">
        <specs beam_angle="30.0"/>
        <range velocity="10.0 10.0 5.0" altitude_min="0.5" altitude_max="50.0"/>
        <noise velocity="0.1" altitude="0.03"/>
        <history samples="1"/>
        <origin xyz="0.0 0.0 0.0" rpy="0.0 0.0 0.0"/>
        <link name="Link1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/scalar/DVL.h>
    sf::DVL* dvl = new sf::DVL("DVL", 30.0, 10.0, 1);
    dvl->setRange(sf::Vector3(10.0, 10.0, 5.0), 0.5, 50.0);
    dvl->setNoise(0.1, 0.03);
    robot->AddLinkSensor(dvl, "Link1", sf::I4());

Profiler
--------

The profiler is a simple acoustic or laser-based device that measures distance to the obstacles by shooting a narrow beam, rotating around an axis, in one plane. Each measurement is a single distance, followed by a change in beam rotation. The specification of the profiler device requires two parameters: the field of view (FOV) and the number of rotation steps. It is also possible to define measured distance limits and measurement noise.

.. code-block:: xml

    <sensor name="Profiler" rate="10.0" type="profiler">
        <specs fov="120.0" steps="128"/>
        <range distance_min="0.5" distance_max="10.0"/>
        <noise distance="0.05"/>
        <history samples="1"/>
        <origin xyz="0.0 0.0 0.0" rpy="0.0 0.0 0.0"/>
        <link name="Link1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/scalar/Profiler.h>
    sf::Profiler* prof = new sf::Profiler("Profiler", 120.0, 128, 10.0, 1);
    prof->setRange(0.5, 10.0);
    prof->setNoise(0.05);
    robot->AddLinkSensor(prof, "Link1", sf::I4());

Multi-beam sonar
----------------

The multi-beam sonar is an acoustic device that measures distance to obstacles by sending acoustic pulses. It utilises multiple acoustic beams, arranged in a planar fan shape. This implementation neglects the beam parameters and resorts to tracing a single ray per beam. More advanced sonar simulations can be found under vision sensors.
The output of the multibeam is a planar distance map, in a cylindrical coordinate system. The specification of the multibeam device requires two parameters: the field of view (FOV) and the number of angle steps (beams). It is also possible to define measured distance limits and measurement noise.

.. code-block:: xml

    <sensor name="Multibeam" rate="1.0" type="multibeam1d">
        <specs fov="120.0" steps="128"/>
        <range distance_min="0.5" distance_max="50.0"/>
        <noise distance="0.1"/>
        <history samples="1"/>
        <origin xyz="0.0 0.0 0.0" rpy="0.0 0.0 0.0"/>
        <link name="Link1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/scalar/Multibeam.h>
    sf::Multibeam* mb = new sf::Multibeam("Multibeam", 120.0, 128, 1.0, 1);
    mb->setRange(0.5, 50.0);
    mb->setNoise(0.1);
    robot->AddLinkSensor(mb, "Link1", sf::I4());

.. _vision-sensors:

Vision sensors
==============

The simulation of vision sensors is based on images generated by the GPU. In case of a typical color camera it means rendering the scene as usual and downloading the frame from the GPU. In case of a more sophisticated sensor like a forward-looking sonar (FLS) it means generating a special input image from the scene data, processing this image to account for the properties of the sensor, and generating an output display image. All processing is fully GPU-based for the ultimate performance. The vision sensors are attached to the robot's links. All of them share the following properties:

1) **Name:** unique string

2) **Rate:** sensor update frequency [Hz] (optional)

3) **Type:** type of the sensor

4) **Origin:** the transformation from the link frame to the sensor frame

5) **Link name**: the name of the robot link that the sensor is attached to

.. code-block:: xml
    
    <sensor name="{1}" rate="{2}" type="{3}">
       <!-- specific definitions here -->
       <origin xyz="{4a}" rpy="{4b}"/>
       <link name="{5}"/>
    </sensor>

.. note::

    Sensor update frequency (rate) is not used in sonar simulations. The actual rate is determined by the maximum sonar range and the sound velocity in water.

Color camera
------------

The color camera is a virtual pinhole camera. The output image is rendered using the standard mode, the same as the visualisation in the main window. 

.. code-block:: xml

    <sensor name="Cam" rate="10.0" type="camera">
        <specs resolution_x="800" resolution_y="600" horizontal_fov="60.0"/>
        <origin xyz="0.0 0.0 0.0" rpy="0.0 0.0 0.0"/>
        <link name="Link1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/vision/ColorCamera.h>
    sf::ColorCamera* cam = new sf::ColorCamera("Cam", 800, 600, 60.0, 10.0);
    robot->AddVisionSensor(cam, "Link1", sf::I4());

Depth camera
------------

The depth camera captures a linear depth image. The output image is a grayscale floating-point bitmap, where black and white colors representing the minimum and maximum captured depth respectively.

.. code-block:: xml

    <sensor name="Dcam" rate="5.0" type="depthcamera">
        <specs resolution_x="800" resolution_y="600" horizontal_fov="60.0" depth_min="0.2" depth_max="10.0"/>
        <origin xyz="0.0 0.0 0.0" rpy="0.0 0.0 0.0"/>
        <link name="Link1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/vision/DepthCamera.h>
    sf::DepthCamera* cam = new sf::DepthCamera("Dcam", 800, 600, 60.0, 0.2, 10.0, 5.0);
    robot->AddVisionSensor(cam, "Link1", sf::I4());

Forward-looking sonar (FLS)
---------------------------

The forward-looking sonar (FLS) is an acoustic device utilising multiple acoustic beams arranged in a planar fan pattern, to generate an acoustic echo intensity map in cylindrical coordinates. This map can be used to detect obstacles or map underwater structures. A characteristic property of this kind of sonar is that the beam width perpendicular to the fan plane is significant, leading to multiple echoes from different beam parts which get projected on the same line.

.. code-block:: xml

    <sensor name="FLS" type="fls">
        <specs beams="512" bins="500" horizontal_fov="120.0" vertical_fov="30.0"/>
        <settings range_min="0.5" range_max="10.0" gain="1.1"/>
        <display colormap="hot"/>
        <origin xyz="0.0 0.0 0.0" rpy="0.0 0.0 0.0"/>
        <link name="Link1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/vision/FLS.h>
    sf::FLS* fls = new sf::FLS("FLS", 512, 500, 120.0, 30.0, 0.5, 10.0, sf::ColorMap::HOT);
    fls->setGain(1.1);
    robot->AddVisionSensor(fls, "Link1", sf::I4());

Mechanical scanning imaging sonar (MSIS)
----------------------------------------

The mechanical scanning imaging sonar (MSIS) is an acoustic device utilising a single rotating acoustic beam. The beam rotates in one plane and generates an acoustic echo intensity map in cylindrical coordinates. This map can be used to detect obstacles or map underwater structures. This kind of sonar produces images similar to the FLS, but due to the rotation of the beam the image is corrupted by the robot's motion.

.. code-block:: xml

    <sensor name="MSIS" type="msis">
        <specs step="0.25" bins="500" horizontal_beam_width="2.0" vertical_beam_width="30.0"/>
        <settings range_min="0.5" range_max="10.0" rotation_min="-50.0" rotation_max="50.0" gain="1.5"/>
        <display colormap="hot"/>
        <origin xyz="0.0 0.0 0.0" rpy="0.0 0.0 0.0"/>
        <link name="Link1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/vision/MSIS.h>
    sf::MSIS* msis = new sf::MSIS("MSIS", 0.25, 500, 2.0, 30.0, -50.0, 50.0, 0.5, 10.0, sf::ColorMap::HOT);
    msis->setGain(1.5);
    robot->AddVisionSensor(msis, "Link1", sf::I4());

Side-scan sonar (SSS)
---------------------

The side-scan sonar (SSS) is an acoutic device with two tranducers, located symmetrically on the robot's hull, with a specified angular separation. The transducers are commonly pointing to the seafloor and allow for fast and detailed mapping of large areas. Each of the transducers emits and receives one beam, creating one line of an acoustic image. The display of the acoustic map is done by adding subsequent lines in a "waterfall" fashion.

.. code-block:: xml

    <sensor name="SSS" type="sss">
        <specs bins="500" lines="400" horizontal_beam_width="2.0" vertical_beam_width="50.0" vertical_tilt="60.0"/>
        <settings range_min="1.0" range_max="100.0" gain="1.2"/>
        <display colormap="hot"/>
        <origin xyz="0.0 0.0 0.0" rpy="0.0 0.0 0.0"/>
        <link name="Link1"/>
    </sensor>

.. code-block:: cpp

    #include <Stonefish/sensors/vision/SSS.h>
    sf::SSS* sss = new sf::SSS("SSS", 500, 400, 50.0, 2.0, 60.0, 1.0, 100.0, sf::ColorMap::HOT);
    sss->setGain(1.2);
    robot->AddVisionSensor(sss, "Link1", sf::I4());