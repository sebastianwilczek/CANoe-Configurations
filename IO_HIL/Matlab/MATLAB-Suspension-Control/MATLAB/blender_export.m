function blender_export()

% The simout-struct variables simout, cpout, cp1dotout and whlbdyvelout
% must be available in the base workspace.
nbodies = 1;

try
    simout = evalin('base', 'simout');
    cpout = evalin('base', 'cpout');
    cp1dotout = evalin('base', 'cp1dotout');
    whlbdyvelout = evalin('base', 'whlbdyvelout');
catch e
    error([e.message, ' Make sure that the variables ''simout'', ''cpout'',', ...
    '''cp1dotout'' and ''whlbdyvelout'' are available in the base workspace.']);
end

export = zeros(size(simout.time,1),nbodies*(6+12+12+12)+1);

% Interpolate on 50 ms grid
xi = linspace(0,simout.time(end),simout.time(end)/0.05+1).';

% === All Angles
% | Translate angles to degrees/10, because blender reads an angle of 1 as 
% | 10 degrees -> multiply with 18/pi;

% Body rotation + Steering angles
export(:,5:9)=unwrap(simout.signals.values(:,4:8))/pi*18; 
% Wheel angles
export(:,14:17)=simout.signals.values(:,13:16)/pi*18;

% === Steering angles
% | Steering is defined as: positive angle -> steer right, but the rotation
% | about the z-axis is actually counter-clockwise -> negate angles
export(:,8:9)=export(:,8:9)*(-1);  

export(:,1)=simout.time;

% === Body positions, suspension deflections, contact point values, wheel
% body velocities
export(:,2:4)=simout.signals.values(:,1:3);
export(:,10:13)=simout.signals.values(:,9:12);
export(:,18:29)=cpout.signals.values;
export(:,30:41)=cp1dotout.signals.values;
export(:,42:53)=whlbdyvelout.signals.values;

% Interpolate the values on the xi sampling points
interpexport = interp1(export(:,1), export(:,2:53), xi, 'linear', 'extrap');

% Form new export matrix
export = [xi, interpexport];

% Save to file as double ascii table
save blender.txt export -ASCII -DOUBLE;

disp('Blender export done.');