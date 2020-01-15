clc;

%=== Simulation time step
T = 0.002;

%=== Load Bus definitions for interfaces between vehicle submodel->root model
load Bus_Vehicle;
load Bus_Measurements;
Bus_VehicleSignals;

%=== Load vehicle Dataset
load VehicleData_VectorCar;

%=== Road roughness filter function
f1 = 0.4; % first corner freq
f2 = 10;  % second corner freq
Q = 0.71; % damping coefficient

Az = tf([4*pi^2*f2^2 0 0],1);
An1 = tf(1,[1 2*pi*f1/Q 4*pi^2*f1^2]);
An2 = tf(1,[1 2*pi*f2/Q 4*pi^2*f2^2]);

RRF = Az*An1*An2; clear Az An1 An2 f1 f2 Q; 

%=== Human response function
s = tf([1 0],[1]);
HRF = 50.698*s*(s+5)*(s^2+28.3*s+2800)*(s^2+105*s+7570)/((s+6)*(s^2+29.8*s+1000)*(s^2+39.1*s+3800)*(s^2+125*s+5180));
clear s;

%=== Generate ground from presets
% Alternatively, set ground_num = 0 and load your own ground dataset from
% a file which contains a ground struct, e.g. "load GroundData_Plane;"
ground_num = 3;

switch ground_num
    case 1 %[1] sinus split, 5cm amplitude, 3 rad/m  
    [X,Y] = meshgrid(linspace(0,64.4,500),[-4 -.025 .25 4]);
    Z=zeros(size(X));
    Z(Y>0)=(cos(X(Y>0)*3+pi)-1)*0.05;
    Z(Y<=0)=(cos(X(Y<=0)*3)-1)*0.05;
    Z(:,end)=Z(:,end-2);
    Z(:,end-1)=Z(:,end-2);
    Z(:,1)=Z(:,3);
    Z(:,2)=Z(:,3);
    Ground.X = X;
    Ground.Y = Y;
    Ground.Z = Z;
    clear X Y Z;
    
    case 2 %[2] sinus, 5cm amplitude, 3 rad/m
    [X,Y] = meshgrid(linspace(0,64.4,500),linspace(-4,4,3));
    Z=(cos(X*3)-1)*0.05;
    Z(:,end)=Z(:,end-2);
    Z(:,end-1)=Z(:,end-2);
    Z(:,1)=Z(:,3);
    Z(:,2)=Z(:,3);
    Ground.X = X;
    Ground.Y = Y;
    Ground.Z = Z;
    clear X Y Z;
    
    case 3 %[3] sinus<->plane, 5cm amplitude, 3 rad/m  
    [X,Y] = meshgrid(linspace(0,64.4,500),[-4 -.025 .25 4]);
    Z=zeros(size(X));
    Z(Y>0)=(cos(X(Y>0)*3+pi)-1)*0.05;
    Z(Y<=0)=0;
    Z(:,end)=Z(:,end-2);
    Z(:,end-1)=Z(:,end-2);
    Z(:,1)=Z(:,3);
    Z(:,2)=Z(:,3);
    Ground.X = X;
    Ground.Y = Y;
    Ground.Z = Z;
    clear X Y Z;
    
    case 4 %[4] ramp 20% inclination starting at X = 32.2
    [X,Y] = meshgrid(linspace(0,64.4,250),[-4 0 4]);   
    Z=zeros(size(X));
    Z(X>32.2)=0.2*(X(X>32.2)-32.2);
    Z(X<=32.2)=0;
    Ground.X = X;
    Ground.Y = Y;
    Ground.Z = Z;
    clear X Y Z;
    
    case 5 %[5] straight plane
    [X,Y] = meshgrid(-4:1:4,-4:1:4);
    Z=zeros(size(X));
    Ground.X = X;
    Ground.Y = Y;
    Ground.Z = Z;
    clear X Y Z;
    
    case 6 %[6] speed bump 10 cm/4 in high, 30 cm/1 ft wide
    [X,Y] = meshgrid(0:0.01:1,-4:1:4);   
    Z=zeros(size(X));
    Z(X>=0.1 & X<=0.25) = (-cos((X(X>=0.1 & X<=0.25)-0.1)/0.15*pi)+1)*0.1/2;
    Z(X>0.25 & X<0.55) = 0.1;
    Z(X>=0.55 & X<=0.70) = (cos((X(X>=0.55 & X<=0.70)-0.55)/0.15*pi)+1)*0.1/2;
    Ground.X = X;
    Ground.Y = Y;
    Ground.Z = Z;
    clear X Y Z;
    
    case 7 %[7] 10cm step
    [X,Y] = meshgrid(10:0.01:11,-4:1:4);
    Z=zeros(size(X));
    Z(X>=10.6) = 0.1;
    Ground.X = X;
    Ground.Y = Y;
    Ground.Z = Z;
    clear X Y Z;
end

% Generate surface normals
[Ground.NormalX, Ground.NormalY, Ground.NormalZ] = surfnorm(Ground.X, Ground.Y, Ground.Z);

blender_export_ground(Ground);  % Export for loading in blender

%=== Recalculate suspension controllers
controller_design;              % Calculate LQR controller