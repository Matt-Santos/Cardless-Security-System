% Convert ECG Data to .mat File for LTSpice

ECG_Functions;

sampleRate = 500;
scale = 0.01/2048;
for i=1:20
  filename = FindECG(i);    % Find ECG Data File (1-lead, limb-clamp, two hands, 12bit)
  [y,t]=ReadECG(filename,sampleRate); % Obtain ECG Data (raw,filtered)
  y_ref = y(2,:);                     % Filtered DataBase Signal
  data = [t',(scale*y_ref)'];
  save(["ECG" num2str(i) ".txt"],"-ascii","data");
end