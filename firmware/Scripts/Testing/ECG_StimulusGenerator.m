% Create Stimulus File for MPLabX

ECG_Functions;

sampleNumber = 6;
sampleRate = 500;

filename = FindECG(sampleNumber);   % Find ECG Data File (1-lead, limb-clamp, two hands, 12bit)
[y,t]=ReadECG(filename,sampleRate); % Obtain ECG Data (raw,filtered)
y_raw = y(1,:);                     % Raw DataBase Signal
y = (y_raw-min(y_raw));             % Remove Offset
y = y/max(y);                       % Normalize from 0 to 1
ECG = {};Temp = {};
for i=1:length(y)
  ECG{i} = dec2hex(ceil(y(i)*1023));% Scale and Convert to Hex
  if mod(i,2) == 0
    Temp{i} = "1F4";
  else
    Temp{i} = "3E8";
  end
end
cnt = 1;Data = {};
fid = fopen("ECG_Stimulus.txt","w");
for i=1:length(y)*2
  if mod(i,2) == 0
    Data{i} = ["0x" Temp{cnt}];
    cnt = cnt+1;
  else
    Data{i} = ["0x" ECG{cnt}];
  end
  fprintf(fid,[Data{i} "\n"]);
end
fclose(fid);
