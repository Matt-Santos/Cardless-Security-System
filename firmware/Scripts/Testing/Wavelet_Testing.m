% Capstone Wavelet Testing
% Written by Matthew Santos
pkg load signal;
pkg load ltfat;
ECG_Functions;
%clear;clc;%close all;
set(0, "defaultlinelinewidth", 1.5);
graphics_toolkit("qt");

% Varriables
sampleRate = 500; %[Hz]
RecordIndex = 3;  %specific sample

% ECG Reference Data (from Database for now)
%-----------------------------------------
filename = FindECG(RecordIndex);    % Find ECG Data File (1-lead, limb-clamp, two hands, 12bit)
[y,t]=ReadECG(filename,sampleRate); % Obtain ECG Data (raw,filtered)
y_ref = y(2,:);                     % Filtered DataBase Signal
y = y(1,:);                         % Raw Unfiltered Signal
y = Fractionalize(y,0.1);


%The Octave Method of undecimated wavelet transform
%--------------------------
% Wavelet Definition
WF_level = 2;
WF_name = 'db1';
%Decomposition
[f5_c,info]=ufwt(y,WF_name,WF_level);
%Coefficient View
figure(2);clf;plot_wavelets(f5_c,t,sampleRate);
%Recomposition
f5Out = iufwt(f5_c(:,1:2),'db1',1)';
%Raw vs Result
figure(1);clf;hold on;plot(y);plot(f5Out);legend("y","f5Out");axis([0 1000]);
%--------------------------


%Attempt to define the transform Manually (for MPU)
%--------------------------
figure(3);clf;wfiltinfo(WF_name); %Visualize the Filters
[w tmp a]= wfilt_db(str2num(WF_name(3:length(WF_name))));  %Obtain the Filter Coeficients in this case db1
LPF = w{1}.h/sqrt(a(1)); %LowPass Impulse Filter
HPF = w{2}.h/sqrt(a(2)); %HighPass Impulse Filter
LPF_offset = -w{1}.offset;%LowPass Time Offset
HPF_offset = -w{2}.offset;%HighPass Time Offset


%Something strange going on with higher orders being scaled down in base case
%Related to A-trous algorithm??

HPResult = y; %Load the inital waveform
for order=1:WF_level
  %Upsample the Filter
  LPF = upsample(LPF,a(1)^(order-1))
  HPF = upsample(HPF,a(2)^(order-1))
  %Pass Previous Level through next level
  LPResult = conv(HPResult,LPF);
  HPResult = conv(HPResult,HPF);
  %Correct for filter delay
  LPResult = LPResult(1+LPF_offset:length(y)+LPF_offset);
  HPResult = HPResult(1+HPF_offset:length(y)+HPF_offset);
  %Store the result
  c(order,:)   = LPResult(1:length(LPResult));
  c(order+1,:) = HPResult(1:length(HPResult));
end
%--------------------------

check = 1;
figure(4);clf;hold on;plot(c(check,:));plot(f5_c(:,check));legend("Test","Base");axis([0 100]);
