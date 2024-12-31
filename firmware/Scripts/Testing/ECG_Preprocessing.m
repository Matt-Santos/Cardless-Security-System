% Capstone ECG Preprocessing Testing
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
ViewMode = 2;     %(1=filters,2=response)

% ECG Reference Data (from Database for now)
%-----------------------------------------
filename = FindECG(RecordIndex);    % Find ECG Data File (1-lead, limb-clamp, two hands, 12bit)
[y,t]=ReadECG(filename,sampleRate); % Obtain ECG Data (raw,filtered)
y_ref = y(2,:);                     % Filtered DataBase Signal
y = y(1,:);                         % Raw Unfiltered Signal

% Preprocessing
%-----------------------------------------
% Low Pass Filter (F<100Hz)
f1_f = [100 125];
f1_M = [1 0];
f1_A = [0.01 0.01]; % ~0.1% deviation
[f1_N,f1_wc,f1_B,f1_type] = kaiserord(f1_f,f1_M,f1_A,sampleRate);
f1Filter = fir1(f1_N,f1_wc,f1_type,kaiser(f1_N+1,f1_B),'noscale');
if ViewMode==1 figure(1);clf;freqz(f1Filter,1,512,sampleRate);title('LP');end
f1Out = filter(f1Filter,1,y);
f1Out = [f1Out(floor(f1_N/2):length(f1Out)) zeros(1,(f1_N/2)-1)]; %Remove delay so we can compare
% DC Blocker (adjust with f2_wc)
f2_wc = 0.995;
f2_a =[1 -f2_wc];
f2_b =[1 -1];
if ViewMode==1 figure(2);clf;freqz(f2_b,f2_a,512,sampleRate);title('DCB');end
f2Out = filter(f2_b,f2_a,f1Out);
% Notch Filter (remove power noise 60Hz, applied twice)
[f3_b,f3_a] = pei_tseng_notch(2*60/sampleRate,4/sampleRate);
if ViewMode==1 figure(3);clf;freqz(f3_b,f3_a,512,sampleRate);title('NF');end
f3Out = filter(f3_b,f3_a,f2Out);
f3Out = filter(f3_b,f3_a,f3Out);
% Moving Average Filter (smoothing)
f4_avg = 8;
f4Filter = ones(f4_avg,1)/f4_avg;
if ViewMode==1 figure(5);clf;freqz(f4Filter,1,512,sampleRate);title('MA');end
f4Out = filter(f4Filter,1,f3Out);
% Wavelet Filter Denoising (WIP - needs tuneing)
f5_level = 3;
f5_name = 'db1';
[f5_c,info]=ufwt(f4Out,f5_name,f5_level);
  %figure();plot_wavelets(f5_c,t,sampleRate);
f5Out = iufwt(f5_c(:,1:2),'db1',1)';



% Plot Preprocessing Results
%-----------------------------------------
Data = [y;f1Out;f2Out;f3Out;f4Out;f5Out];
Labels = {'LowPass','DCBlocker','NotchFilter','AveragingFilter','Wavelet Denoise'};
if ViewMode==2
  for i = 1:size(Data,1)-1
    figure(i);clf;
    subplot(2,1,1);
    plot(t,Data(i,:));hold on;
    plot(t,Data(i+1,:));title([Labels(i) 'Time Signal']);
    xlabel('Time [s]');ylabel('12bit Rez');
    axis([0 1.5 -inf inf]);
    subplot(2,1,2);
    Get_FFT(Data(i,:),sampleRate);hold on;
    Get_FFT(Data(i+1,:),sampleRate);title([Labels(i) 'FFT']);
  end
end

% Compare Results
figure(6);clf;
plot(t,y);hold on;title('Raw vs Processed');
plot(t,f5Out);axis([0 1.5]);
figure(7);clf;
plot(t,y_ref);hold on; plot(t,f5Out);axis([0 1.5]);title('RawFiltered vs Processed');
