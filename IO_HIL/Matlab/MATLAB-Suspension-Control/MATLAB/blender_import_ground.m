function Ground = blender_import_ground(file)

try
    S = load(file,'-ascii');
catch e
    error(e.message)
    return;
end

XYZData = sortrows(S);

F = TriScatteredInterp(XYZData(:,1),XYZData(:,2),XYZData(:,3));

[Ground.X,Ground.Y]=meshgrid(min(XYZData(:,1)):0.30:max(XYZData(:,1)), ...
    min(XYZData(:,2)):0.30:max(XYZData(:,2)));
Ground.Z = F(Ground.X,Ground.Y);
Ground.Z = inpaint_nans(Ground.Z,4);
[Ground.NormalX, Ground.NormalY, Ground.NormalZ] = surfnorm(Ground.X, Ground.Y, Ground.Z);

figure;clf;mesh(Ground.X,Ground.Y,Ground.Z);xlabel('X');ylabel('Y');zlabel('Z');
set(gca, 'DataAspectRatio',[1 1 1]);