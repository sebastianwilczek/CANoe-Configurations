function blender_export_ground( Ground )

% Copy variables for writing convenience
X = Ground.X;
Y = Ground.Y;
Z = Ground.Z;

% Extrapolate ground linearly by dist
dist = 100;

Xi=[X(1,1)-dist*ones(size(X,1)+2,1), [X(1,:);X;X(end,:)], X(1,end)+dist*ones(size(X,1)+2,1)];
Yi=[Y(1,1)-dist*ones(1,size(Y,2)+2); [Y(:,1),Y,Y(:,end)]; Y(end,1)+dist*ones(1,size(Y,2)+2)];
Zio = Z(1,:)+(Z(1,:)-Z(2,:))/(Y(2,1)-Y(1,1))*dist;
Zil = Z(:,1)+(Z(:,1)-Z(:,2))/(X(1,2)-X(1,1))*dist;
Zir = Z(:,end)+(Z(:,end)-Z(:,end-1))/(X(1,end)-X(1,end-1))*dist;
Ziu = Z(end,:)+(Z(end,:)-Z(end-1,:))/(Y(end-1,1)-Y(end,1))*dist;

porl=[Xi(1,end-1);Yi(1,end-1);Zio(end)];
porr=[Xi(2,end);Yi(2,end);Zir(1)];
por=[X(1,end);Y(1,end);Z(1,end)];
vor = (.5*(porl-porr)+porr-por)*2+por;
Zior = vor(3);

polr=[Xi(1,2);Yi(1,2);Zio(1)];
poll=[Xi(2,1);Yi(2,1);Zil(1)];
pol=[X(1,1);Y(1,1);Z(1,1)];
vol = (.5*(polr-poll)+poll-pol)*2+pol;
Ziol = vol(3);

pulr=[Xi(end,2);Yi(end,2);Ziu(1)];
pull=[Xi(end-1,1);Yi(end-1,1);Zil(end)];
pul=[X(end,1);Y(end,1);Z(end,1)];
vul = (.5*(pull-pulr)+pulr-pul)*2+pul;
Ziul = vul(3);

purl=[Xi(end,end-1);Yi(end,end-1);Ziu(end)];
purr=[Xi(end-1,end);Yi(end-1,end);Zir(end)];
pur=[X(end,end);Y(end,end);Z(end,end)];
vur = (.5*(purl-purr)+purr-pur)*2+pur;
Ziur = vur(3);

Zi=[Ziol,Zio,Zior;Zil,Z,Zir;Ziul,Ziu,Ziur];

export = zeros(size(Zi,2)*size(Zi,1),3);

for j=1:size(Zi,1)
    for i=1:size(Zi,2)
        export((j-1)*size(Zi,2)+i,1) = Xi(j,i);
        export((j-1)*size(Zi,2)+i,2) = Yi(j,i);
        export((j-1)*size(Zi,2)+i,3) = Zi(j,i);
    end
end

faceslist = zeros((size(Zi,1)-1)*(size(Zi,2)-1),4);

count = 0;
for j=1:(size(Zi,1)-1)
    for i=1:(size(Zi,2)-1)
        faceslist((j-1)*(size(Zi,2)-1)+i,1) = count;
        faceslist((j-1)*(size(Zi,2)-1)+i,2) = count+1;
        faceslist((j-1)*(size(Zi,2)-1)+i,3) = count+size(Zi,2)+1;
        faceslist((j-1)*(size(Zi,2)-1)+i,4) = count+size(Zi,2);
        count = count+1;
    end
    count = count+1;
end
save blender_ground.txt export faceslist -ASCII -DOUBLE;

disp('Blender export ground done.');