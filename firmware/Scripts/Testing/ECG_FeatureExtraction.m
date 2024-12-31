% Capstone ECG Feature Extraction
% Written by Matthew Santos
pkg load signal;
pkg load ltfat;
ECG_Functions;
clear;clc;%close all;
set(0, "defaultlinelinewidth", 1.5);
graphics_toolkit("qt");

% Varriables
sampleRate = 500; %[Hz]
RecordIndex = 18;  

% ECG Reference Data (Just use Filtered for Testing)
%-----------------------------------------
filename = FindECG(RecordIndex);    % Find ECG Data File (1-lead, limb-clamp, two hands, 12bit)
[y,t]=ReadECG(filename,sampleRate); % Obtain ECG Data (raw,filtered)
y_ref = y(2,:);                     % Filtered DataBase Signal
y = y(1,:);                         % Raw Unfiltered Signal

% R-Peak Detection (wavelet part could use a little tuning)
%-----------------------------------------
[C,info] = ufwt(y_ref,'db5',6);
RPeakWaveform = C(:,4);
RPeakWaveform(RPeakWaveform>0)= 0;
RPeakWaveform = RPeakWaveform.^2;
MinRHeight = 0.2;
MinRDistance = sampleRate*0.3;
[RPeakLoc,RPeakVal] = findPeaks(RPeakWaveform,y_ref,MinRHeight*max(RPeakWaveform),MinRDistance);
for index=1:length(RPeakLoc)
  for index2=-2:2
    if y_ref(RPeakLoc(index))<y_ref(RPeakLoc(index)+index2)
      RPeakLoc(index) = RPeakLoc(index)+index2; %extra fine tuning
    end
  end
end
DeltaRPeak = RPeakLoc-[0 RPeakLoc(1:length(RPeakLoc)-1)];
DeltaRPeak = DeltaRPeak(2:length(DeltaRPeak));


% Perform Sectioning (and Normalization)
%-----------------------------------------
sectionOffset = round(0.5*min(DeltaRPeak));
sectionMin = 2*sectionOffset;
section = RPeakLoc(2:length(RPeakLoc)-1)-sectionOffset;
for index=1:length(DeltaRPeak)-1
  sections(:,index) = y_ref(section(index):section(index)+sectionMin)/y_ref(RPeakLoc(index+1));
end
sections = dct(sections);
sections = idct(sections,sampleRate);
  
% Establish Section Average and Stdev
%-----------------------------------------
SectionAverage = mean(sections')';
SectionVarrience = std(sections')';



% Diagonstic Plots
%-----------------------------------------
figure(1);clf;plot(t,y_ref);hold on;
  plot(t(RPeakLoc),y_ref(RPeakLoc),'ro');
  patchX = [t(section); t(section+sectionMin); t(section+sectionMin); t(section)];
  patchY = [ones(1,length(section))*min(y_ref); ones(1,length(section))*min(y_ref); ones(1,length(section))*max(y_ref); ones(1,length(section))*max(y_ref)];
  patch(patchX,patchY,'cyan');
  title('R-Peaks and Section Zones');
figure(2);clf;
  subplot(2,1,1);hold on;plot(sections);
  plot(SectionAverage,'linewidth',4,'r');title('Sections');
  subplot(2,1,2);plot(SectionVarrience,'linewidth',4,'r');title('Standerd Deviation');



% Feature List
%-----------------------------------------
  %RPeakLoc %sample numbers of R Peaks
  %DeltaRPeak %Distance between R Peaks
  AverageHR = sampleRate/mean(DeltaRPeak)*60
  HRVarriablity = sqrt(60*sampleRate/mean((DeltaRPeak(2:length(DeltaRPeak))-DeltaRPeak(1:length(DeltaRPeak)-1)).^2))
  %SectionAverage
  %SectionVarrience