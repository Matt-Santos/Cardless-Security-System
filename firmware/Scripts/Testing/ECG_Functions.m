1;
function [y t]=ReadECG(filename,sampleRate)
  fid=fopen(filename,'r');
  if fid==-1
    disp('Error Reading File');  
    y=0;t=0;
    fclose(fid);
    return;
  end
  y=fread(fid,[2,20*sampleRate],'int16');
  t=0:1/sampleRate:length(y)/sampleRate;
  if length(t)>length(y)
    t = t(1:length(y));
  elseif length(y)>length(t)
    y = y(1:length(t));
  end
  fclose(fid);
end

function filename = FindECG(RecordIndex)
  fid = fopen('../ECGSamples/RECORDS');
  for i=0:RecordIndex
    filename = ['../ECGSamples/' fgetl(fid) '.dat'];
  end
  fclose(fid);
end

function y = Fractionalize(data,percentHeadRoom)
  Dmax = max(data);
  Dmin = min(data);
  y = 2*(data-Dmin)/(Dmax-Dmin)-1;
  y = (1-percentHeadRoom)*y;
end

function [x,y] = Get_FFT(signal,sf);
  mag = abs(fftshift(fft(signal)/length(signal)));
  y=20*log10(mag/max(mag));
  x = sf*(-length(y)/2:length(y)/2-1)/length(y);
  if (nargout==0)
    plot(x,y);
    xlabel("Frequency [Hz]");
    ylabel("Magnitude [dB]");
    limit = -80;
    tmp = find(y >= limit);
    axis([0 x(tmp(length(tmp))) limit 0]);
  end
end

function y = plot_wavelets(c,t,fs)
  figure(2);
  for k=1:size(c,2)
    subplot(size(c,2),2,2*k-1);plot(t,c(:,k));
    axis([0 t(length(t)) min(min(c)) max(max(c))]);
    subplot(size(c,2),2,2*k);Get_FFT(c(:,k),fs);
  end
end
%Custom Peak Finding Algorithm (MPU Friendly)
function [Loc,Amp] = findPeaks(x,y,MinHeight,MinDistance);
  DFilter = [-1 8 -8 1]./12;  %standard 5-point descrete derivative FIR
  DWaveform = filter(DFilter,1,x);  %Take Derivative
  Loc = [];Amp = [];
  for i=1:length(DWaveform)-1
    if DWaveform(i)*DWaveform(i+1)<0
      if x(i)>=MinHeight
        Loc = [Loc i];
        Amp = [Amp x(i)];
      end
    end
  end
  i=1;
  while (i<length(Loc))
    if Loc(i+1)-Loc(i)<MinDistance
      if x(Loc(i))>x(Loc(i+1)) %Need regular signal here?
        Loc(i+1) =[];Amp(i+1) = [];
      else
        Loc(i) =[];Amp(i) = [];
      end
      i = i-1;
    end
    i = i+1;
  end
end
