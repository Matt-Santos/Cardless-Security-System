% Capstone ECG Functions
% Written by Matthew Santos
1;
%FindECG, returns the filename of the ecg record
function filename = FindECG(RecordIndex,unique)
  if unique
    fid = fopen('../ECGSamples/U_RECORDS');
  else
    fid = fopen('../ECGSamples/RECORDS');
  end
  for i=0:RecordIndex
    filename = ['../ECGSamples/' fgetl(fid) '.dat'];
  end
  fclose(fid);
end
%ReadECG, returns the ecg data of the record index
function [y t]=ReadECG(RecordIndex,sampleRate)
  filename = FindECG(RecordIndex,1);%unique records
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
%Fractionalize, converts data to fractional data type
function y = Fractionalize(data,percentHeadRoom)
  Dmax = max(data);
  Dmin = min(data);
  y = 2*(data-Dmin)/(Dmax-Dmin)-1;
  y = (1-percentHeadRoom)*y;
end
%Get_FFT, plot the correct fft for a signal
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
%plot_wavelets, used to plot wavelet coeficients for testing
function y = plot_wavelets(c,fs)
  for k=1:size(c,2)
    subplot(size(c,2),2,2*k-1);plot(c(:,k));
    axis([0 length(c) min(min(c)) max(max(c))]);
    subplot(size(c,2),2,2*k);Get_FFT(c(:,k),fs);
  end
end
%Preprocessing, perform complete ecg preprocessing
function [CleanECG PPFilter] = Preprocessing(RawECG,sampleRate)
  % Low Pass Filter (F<100Hz)
  f1_f = [100 125];
  f1_M = [1 0];
  f1_A = [0.01 0.01];
  [f1_N,f1_wc,f1_B,f1_type] = kaiserord(f1_f,f1_M,f1_A,sampleRate);
  f1_b = fir1(f1_N,f1_wc,f1_type,kaiser(f1_N+1,f1_B),'noscale');
  % DC Blocker (adjust with f2_wc)
  f2_wc = 0.995;
  f2_a =[1 -f2_wc];
  f2_b =[1 -1];
  % Notch Filter (remove power noise 60Hz, applied twice)
  [f3_b,f3_a] = pei_tseng_notch(2*60/sampleRate,4/sampleRate);
  % Moving Average Filter (smoothing)
  f4_avg = 8;
  f4_b = ones(f4_avg,1)/f4_avg;
  % Wavelet Filter Denoising (WIP - needs tuneing)
  f5_level = 3;
  f5_name = 'db1';
  %Apply the Filters
  PPFilterB = conv(conv(conv(conv(f1_b,f2_b),f3_b),f3_b),f4_b)
  PPFilterA = conv(conv(f2_a,f3_a),f3_a)
  f4Out = filter(PPFilterB,PPFilterA,RawECG);
  
  [f5_c,info]=ufwt(f4Out,f5_name,f5_level);
  f5Out = iufwt(f5_c(:,1:2),'db1',1)';
  %Must Correct for Delay %f1Out = [f1Out(floor(f1_N/2):length(f1Out)) zeros(1,(f1_N/2)-1)]; %Remove delay so we can compare
  CleanECG = f5Out;
  
  PPFilter = tf2sos(PPFilterB,PPFilterA)
end
%findPeaks, Custom Peak Finding Algorithm (MPU Friendly)
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
      if x(Loc(i))>x(Loc(i+1))
        Loc(i+1) =[];Amp(i+1) = [];
      else
        Loc(i) =[];Amp(i) = [];
      end
      i = i-1;
    end
    i = i+1;
  end
end
%Sectioning, seperates ecg segments and overlaps them to form an average
function [Left,Right,avgECG] = Sectioning(CleanECG,sampleRate,RPeakLoc)
  DeltaRPeak = RPeakLoc(2:length(RPeakLoc))-RPeakLoc(1:length(RPeakLoc)-1);
  sectionOffset = round(0.5*min(DeltaRPeak));
  Left = RPeakLoc(2:length(RPeakLoc)-1)-sectionOffset;
  sectionMin = min(Left(3:length(Left)-1)-[Left(2:length(Left)-2)]);
  Right = Left + sectionMin;
  for index=1:length(DeltaRPeak)-1
    sections(:,index) = CleanECG(Left(index):Right(index))/CleanECG(RPeakLoc(index+1));
  end
  %Diagnosing Sectioning
    %figure(1);clf;hold on;plot(CleanECG);plot(RPeakLoc,CleanECG(RPeakLoc),'ro');
    %patchX = [Left; Right; Right; Left];
    %patchY = [ones(1,length(Left))*min(CleanECG); ones(1,length(Left))*min(CleanECG); ones(1,length(Left))*max(CleanECG); ones(1,length(Left))*max(CleanECG)];
    %patch(patchX,patchY,'cyan');
  %Period Normalization (Just truncate, ECG doesn't seem to normalize well via HR)
    %sections = dct(sections);
    %sections = idct(sections,sampleRate);
  avgECG = mean(sections')';
end
%RPeakDetector, returns the sample numbers of all ecg R peaks
function RPeakLoc = RPeakDetector(CleanECG,sampleRate)
  [C,info] = ufwt(CleanECG,'db5',6);
  RPeakWaveform = C(:,4);
  RPeakWaveform(RPeakWaveform>0)= 0;
  RPeakWaveform = abs(RPeakWaveform); %Used to use square but not good with fixed point
  MinRHeight = 0.2;
  MinRDistance = sampleRate*0.3;
  [RPeakLoc,RPeakVal] = findPeaks(RPeakWaveform,CleanECG,MinRHeight*max(RPeakWaveform),MinRDistance);
  for index=1:length(RPeakLoc)
    for index2=-2:2
      if CleanECG(RPeakLoc(index))<CleanECG(RPeakLoc(index)+index2)
        RPeakLoc(index) = RPeakLoc(index)+index2; %extra fine tuning
      end
    end
  end
end
%PPeakDetector, returns the sample numbers of all ecg P peaks
function P = PPeakDetector(CleanECG,sampleRate,R);
  PEndOffset = 15;  %window offset from R
  if length(R)==1
    PPeakWaveform = CleanECG(1:R-PEndOffset)-min(CleanECG);
  else
    DeltaRPeak = R(2:length(R))-R(1:length(R)-1);
    offset = round(0.5*min(DeltaRPeak));
    PPeakWaveform = CleanECG-min(CleanECG);
    for i=2:length(R)
      PPeakWaveform(R(i)-PEndOffset:R(i)+offset) = 0;
    end
    PPeakWaveform(1:R(1)+offset) = 0; %Zero left end
    PPeakWaveform(R(length(R)-1)-PEndOffset:length(PPeakWaveform)) = 0; %Zero right end
  end
  PPeakWaveform = (10*PPeakWaveform).^2;
  MinPHeight = 0.20;
  MinPDistance = sampleRate*0.50;
  [P,tmp] = findPeaks(PPeakWaveform,CleanECG,MinPHeight,MinPDistance);
  %figure(3);clf;hold on;plot(0.01*PPeakWaveform);plot(CleanECG);plot(P,CleanECG(P),'ro');
end
%QPeakDetector, returns the sample numbers of all ecg Q peaks
function Q = QPeakDetector(CleanECG,sampleRate,R);
  QEndOffset = 30;  %window offset from R
  if length(R)==1
    QPeakWaveform = [zeros(R-QEndOffset-1,1); CleanECG(R-QEndOffset:R)-1];
  else
    DeltaRPeak = R-[0 R(1:length(R)-1)];
    DeltaRPeak = DeltaRPeak(2:length(DeltaRPeak));
    offset = round(0.5*min(DeltaRPeak));
    QPeakWaveform = CleanECG-1;
    for i=1:length(R)-1
      QPeakWaveform(R(i):R(i+1)-QEndOffset) = 0;
    end
    QPeakWaveform(1:R(1)) = 0;
    QPeakWaveform(R(length(R)-1):length(QPeakWaveform)) = 0;
  end
  QPeakWaveform = QPeakWaveform*-1;
  QPeakWaveform = (10*QPeakWaveform).^2;
  MinQHeight = 0.5;
  MinQDistance = sampleRate*0.5;
  [Q,tmp] = findPeaks(QPeakWaveform,CleanECG,MinQHeight,MinQDistance);
end
%SPeakDetector, returns the sample numbers of all ecg Q peaks
function S = SPeakDetector(CleanECG,sampleRate,R);
  SEndOffset = 30;  %window offset from R
  if length(R)==1
    SPeakWaveform = [zeros(R-1,1); CleanECG(R:R+SEndOffset)-1];
  else
    DeltaRPeak = R-[0 R(1:length(R)-1)];
    DeltaRPeak = DeltaRPeak(2:length(DeltaRPeak));
    offset = round(0.5*min(DeltaRPeak));
    SPeakWaveform = CleanECG-1;
    for i=1:length(R)-1
      SPeakWaveform(R(i)+SEndOffset:R(i+1)) = 0;
    end
    SPeakWaveform(1:R(2)) = 0;
    SPeakWaveform(R(length(R)):length(SPeakWaveform)) = 0;
  end
  SPeakWaveform = SPeakWaveform*-1;
  SPeakWaveform = (10*SPeakWaveform).^2;
  MinSHeight = 0.5;
  MinSDistance = sampleRate*0.5;
  [S,tmp] = findPeaks(SPeakWaveform,CleanECG,MinSHeight,MinSDistance);
end
%TPeakDetector, returns the sample numbers of all ecg P peaks
function T = TPeakDetector(CleanECG,sampleRate,R);
  TEndOffset = 15;  %window offset from R
  if length(R)==1
    TPeakWaveform = [zeros(R+TEndOffset-1,1); CleanECG(R+TEndOffset:length(CleanECG))-min(CleanECG)];
  else
    DeltaRPeak = R-[0 R(1:length(R)-1)];
    DeltaRPeak = DeltaRPeak(2:length(DeltaRPeak));
    offset = round(0.5*min(DeltaRPeak));
    TPeakWaveform = CleanECG-min(CleanECG);
    for i=2:length(R)
      TPeakWaveform(R(i)-offset:R(i)+TEndOffset) = 0;
    end
    TPeakWaveform(1:R(2)) = 0; %Zero left end
    TPeakWaveform(R(length(R))-TEndOffset:length(TPeakWaveform)) = 0; %Zero right end
  end
  TPeakWaveform = (10*TPeakWaveform).^2;
  MinTHeight = 0.2;
  MinTDistance = sampleRate*0.5;
  [T,tmp] = findPeaks(TPeakWaveform,CleanECG,MinTHeight,MinTDistance);
end
%FeatureCalculations, obtain feature values (interrelations)
function Features = FeatureCalculations(CleanECG,P,Q,R,S,T,DeltaRPeak,sampleRate)
  DeltaDeltaRPeak = DeltaRPeak(2:length(DeltaRPeak))-DeltaRPeak(1:length(DeltaRPeak)-1);
  if length(R)==1
    HR = mean(sampleRate./[DeltaRPeak]*60);
    HRVarriablity = mean(abs([DeltaDeltaRPeak]));
  else
    HR = sampleRate./[DeltaRPeak(1:length(DeltaRPeak)-1)]*60;
    HRVarriablity = abs([DeltaDeltaRPeak]);
  end
  QR_T = Q-R;
  RS_T = R-S;
  QT_T = Q-T;
  PS_T = P-S;
  PQ_T = P-Q;
  ST_T = S-T;
  RT_T = R-T;
  PR_T = P-R;
  PT_T = P-T;
  QR_S = (CleanECG(Q)-CleanECG(R))./(QR_T);
  RS_S = (CleanECG(R)-CleanECG(S))./(RS_T);
  PT_S = (CleanECG(P)-CleanECG(T))./(PT_T);
  PR_S = (CleanECG(P)-CleanECG(R))./(PR_T);
  RT_S = (CleanECG(R)-CleanECG(T))./(RT_T);
  PQ_S = (CleanECG(P)-CleanECG(Q))./(PQ_T);
  ST_S = (CleanECG(S)-CleanECG(T))./(ST_T);
  QT_S = (CleanECG(Q)-CleanECG(T))./(QT_T);
  PS_S = (CleanECG(P)-CleanECG(S))./(PS_T);
  RP_A = (CleanECG(R)-CleanECG(P));
  PT_A = (CleanECG(P)-CleanECG(T));
  PQ_A = (CleanECG(P)-CleanECG(Q));
  TS_A = (CleanECG(T)-CleanECG(S));
  RQ_A = (CleanECG(R)-CleanECG(Q));
  RS_A = (CleanECG(R)-CleanECG(S));
  QS_A = (CleanECG(Q)-CleanECG(S));
  Features = [...
    HR;...                       %Heart Rate
    HRVarriablity;...            %Heart Rate Varriability
    QR_T;RS_T;QT_T;PS_T;PQ_T;... %Periods
    ST_T;RT_T;PR_T;PT_T;...      %Periods
    QR_S;RS_S;PT_S;PR_S;RT_S;... %Slopes
    PQ_S;ST_S;QT_S;PS_S;...      %Slopes
    RP_A;PT_A;PQ_A;TS_A;RQ_A;... %Amplitudes
    RS_A;QS_A...                 %Amplitudes
    ];
end
%FeatureExtraction, extract features from the filtered ECG signal
function Features = FeatureExtraction(CleanECG,sampleRate,testMode)
  R = RPeakDetector(CleanECG,sampleRate);
  DeltaRPeak = R(2:length(R))-R(1:length(R)-1);
  if testMode == 1 %Perform Sectioning
    [tmp,tmp,CleanECG] = Sectioning(CleanECG,sampleRate,R); %Get AverageWaveform
    R = round(0.5*min(DeltaRPeak)); %Obtain Avg R Peak
  else
    [Left,Right,tmp] = Sectioning(CleanECG,sampleRate,R);
    CleanECG = CleanECG/max(CleanECG);  %Normalize Overall Amplitude
    for index=1:length(DeltaRPeak)-1  %Normalize Segments to Peak
      CleanECG(Left(index):Right(index)) = CleanECG(Left(index):Right(index))/CleanECG(R(index+1));
    end
  end
  P = PPeakDetector(CleanECG,sampleRate,R);
  Q = QPeakDetector(CleanECG,sampleRate,R);
  S = SPeakDetector(CleanECG,sampleRate,R);
  T = TPeakDetector(CleanECG,sampleRate,R);
  if length(R)>1 R= R(2:length(R)-1);end
  %Diagnosing Peak Detections
    %figure(1);clf;hold on;
    %plot(CleanECG);
    %plot(P,CleanECG(P),'bo');printf("P=%d\n",length(P));
    %plot(Q,CleanECG(Q),'ro');printf("Q=%d\n",length(Q));
    %plot(R,CleanECG(R),'ro');printf("R=%d\n",length(R));
    %plot(S,CleanECG(S),'ro');printf("S=%d\n",length(S));
    %plot(T,CleanECG(T),'go');printf("T=%d\n",length(T));
    %pause();
    Features = FeatureCalculations(CleanECG,P,Q,R,S,T,DeltaRPeak,sampleRate);
end
%NormalizeFeatures, normalize according to F=(F-mean)/std
function Features = NormalizeFeatures(Features,RecordNumbers,Mean,Std)
  if length(RecordNumbers) == 1
    Features = (Features-Mean)./Std;  
  else
    for i=RecordNumbers
      Features{i} = (Features{i}-Mean)./Std;  
    end
  end
end
%GetFeatureNormFactors, calculate the feature normalization factors
function [Mean,Std] = GetFeatureNormFactors(Features,RecordNumbers)
  Mean =[];Std =[];
  for i=RecordNumbers
    Mean = [Mean mean(Features{i}')'];
    Std  = [Std std(Features{i}')'];
  end
  Mean = mean(Mean')';
  Std = std(Std')';
end
%AssembleSVM_Matrix, put features in format for libsvm
function [ClassLabels,TrainingMatrix] = AssembleSVM_Matrix(Features,RecordNumbers,Record);
  TrainingMatrix = [];ClassLabels = [];
  for i=RecordNumbers  %RecordNumbers
    temp = Features{i};
    for ii=1:size(Features{i},2)  %Features
      if i==Record  %Possitive Results
        TrainingMatrix = [temp(:,ii)' ; TrainingMatrix ];
        ClassLabels = [1 ; ClassLabels];
      else          %Negative Results
        TrainingMatrix = [TrainingMatrix ; temp(:,ii)'];
        ClassLabels = [ClassLabels ; -1];
      end
    end
  end
  TrainingMatrix = sparse(TrainingMatrix);
end
%GetClassifiers, obtain list of all OvR classifier components
function Classifiers = GetClassifiers(Features,RecordNumbers);
  Classifiers=[]; Name=['bob' num2str(i)];
  for i=RecordNumbers
    [ClassLabels,TrainingMatrix] = AssembleSVM_Matrix(Features,RecordNumbers,i);
    SVM = svmtrain(ClassLabels,TrainingMatrix);
    temp = [...
      i                        ...  %Classifier Label
      %Name                    ...  %Name (WIP)
      SVM.Parameters(4)        ...  %gamma
      -SVM.rho                 ...  %b
      size(SVM.SVs)            ...  %[nSVs,nF]
      full(SVM.sv_coef')       ...  %w
      full(SVM.SVs'(:)')       ...  %SVs
    ];
    CurrentSize = size(Classifiers,2);
    NewSize = length(temp);
    if NewSize>CurrentSize
      Classifiers = [Classifiers zeros(size(Classifiers,1),NewSize-CurrentSize)];
    elseif NewSize<CurrentSize
      temp = [temp zeros(CurrentSize-NewSize,1)'];
    end
    Classifiers = [Classifiers;temp];
  end
end
%GetFscores, obtain the fscores for each Record
function Fscores = GetFscores(Features,RecordNumbers)
  Fscores =[];
  for i=RecordNumbers
    [CL,TM] = AssembleSVM_Matrix(Features,RecordNumbers,i);
    TM=full(TM);
    T_p=[];T_n=[];
    for ii=1:length(CL)
      if CL(ii)==1
        T_p = [T_p;TM(ii,:)];
      else      
        T_n = [T_n;TM(ii,:)];
      end
    end
    T = [T_p;T_n];
    Delta_p = sum((T_p'-mean(T_p',2)).^2,2)./(size(T_p,2)-1);
    Delta_n = sum((T_n'-mean(T_n',2)).^2,2)./(size(T_n,2)-1);
    Fscore = (sum((T_p'-mean(T',2)).^2,2)+sum((T_n'-mean(T',2)).^2,2))./(Delta_p+Delta_n);
    Fscores = [Fscores;Fscore'];
  end
end
%SaveFeatureNorms, store feature mean and std data into file
function SaveFeatureNorms(file,NumRecords,Mean,Std)
  fid = fopen(file,'w');
  fwrite(fid,[NumRecords Mean' Std'],'float32',0,'ieee-be');
  fclose(fid);
end
%LoadFeatureNorms, load feature mean and std data from file
function [NumRecords,Mean,Std] = LoadFeatureNorms(file,N_Features)
  fid = fopen(file,'r');
  data = fread(fid,[Inf],'float32',0,'ieee-be');
  NumRecords = data(1);
  Mean = data(2:N_Features+1);
  Std = data(N_Features+2:length(data)); 
  fclose(fid);
end
%SaveClassifiers, store resulting SVM data into file
function SaveClassifiers(file,Classifiers)
  fid = fopen(file,'w');
  fwrite(fid,Classifiers,'float32',0,'ieee-be');
  fclose(fid);
end
%LoadClassifiers, load SVM data from file
function Classifiers = LoadClassifiers(file,N_Records)
  fid = fopen(file,'r');
  Classifiers = fread(fid,[N_Records,Inf],'float32',0,'ieee-be');
  fclose(fid);
end
%TestClassifier, test a feature vector vs all classifiers
function Result = TestClassifiers(TestFeatures,NumRecords,file)
  Result = [];
  Classifiers = LoadClassifiers(file,NumRecords);
  for i=1:NumRecords %Each Classifier
    % Get SVM Parameters for Single Classifier
    label = Classifiers(i,1);
    gamma = Classifiers(i,2);
    b     = Classifiers(i,3);
    nSVs  = Classifiers(i,4);
    nF    = Classifiers(i,5);
    w     = Classifiers(i,6:5+nSVs)';
    SV    = reshape(Classifiers(i,6+nSVs:5+nSVs+nSVs*nF),nF,nSVs)';
    %Calculate the RBF Function
    D=[];Q=[];
    for ii=1:nSVs
      D(ii) = norm(SV(ii,:)-TestFeatures);
      Q(ii) = exp(-gamma*D(ii).^2);
    end
    Result = [Result;Q*w+b]; %Score for each Classifier
  end
end
%AuthorizeCheck, used for determining if a result warrents opening the door
function [Access Identity Margin] = AuthorizeCheck(Scores)
  Access = 0;
  [TopScore Identity] = max(Scores);
  tmp = sort(Scores);
  Margin = Scores - tmp(2);
  if (TopScore >= 0 && TopScore-tmp(2) > 0.5)
    Access = 1;
  end
end
%PlotClassifier, used to plot 2D classifier Result
function PlotClassifier(ClassifierID,FeatureX,FeatureY,Classifiers,Features)
  XL=[];XH=[];YL=[];YH=[];
  Features= Features(~cellfun('isempty',Features));
  for i=1:size(Classifiers,1) %Classes
    temp = Features{i};
    XL = min([XL min(temp(FeatureX,:))]);
    XH = max([XH max(temp(FeatureX,:))]);
    YL = min([YL min(temp(FeatureY,:))]);
    YH = max([YH max(temp(FeatureY,:))]);
    if i==ClassifierID
      plot(temp(FeatureX,:),temp(FeatureY,:),'kd');
    else
      plot(temp(FeatureX,:),temp(FeatureY,:),'rx');
    end
  end
  px = XL:(XH-XL)/25:XH;
  py = YL:(YH-YL)/25:YH;
  gamma = Classifiers(ClassifierID,2);
  b     = Classifiers(ClassifierID,3);
  nSVs  = Classifiers(ClassifierID,4);
  nF    = Classifiers(ClassifierID,5);
  w     = Classifiers(ClassifierID,6:5+nSVs)';
  SV    = reshape(Classifiers(ClassifierID,6+nSVs:5+nSVs+nSVs*nF),nF,nSVs)';
  SV = SV(:,[FeatureX FeatureY]);
  for X = 1:length(px)  
    for Y = 1:length(py)
      D=[];Q=[];TestFeatures = [px(X) py(Y)];
      for ii=1:nSVs
        D(ii) = norm(SV(ii,:)-[TestFeatures]);
        Q(ii) = exp(-gamma*D(ii).^2);
      end
      pz(Y,X) = Q*w+b;
    end
  end  
  colormap ("jet");
  contourf(px,py,pz);
  colorbar ();
  axis([XL XH YL YH]);
end
%PlotClassifier3D, used to plot 3D classifier Result
function PlotClassifier3D(ClassifierID,FeatureX,FeatureY,Classifiers,Features)
  XL=[];XH=[];YL=[];YH=[];hold on;
  Features= Features(~cellfun('isempty',Features));
  %Get Classifier Data
  gamma = Classifiers(ClassifierID,2);
  b     = Classifiers(ClassifierID,3);
  nSVs  = Classifiers(ClassifierID,4);
  nF    = Classifiers(ClassifierID,5);
  w     = Classifiers(ClassifierID,6:5+nSVs)';
  SV    = reshape(Classifiers(ClassifierID,6+nSVs:5+nSVs+nSVs*nF),nF,nSVs)';
  SV = SV(:,[FeatureX FeatureY]);
  for i=1:size(Classifiers,1) %Classes
    temp = Features{i};
    XL = min([XL min(temp(FeatureX,:))]);
    XH = max([XH max(temp(FeatureX,:))]);
    YL = min([YL min(temp(FeatureY,:))]);
    YH = max([YH max(temp(FeatureY,:))]);
  end
  px = XL:(XH-XL)/100:XH;
  py = YL:(YH-YL)/100:YH;
  for X = 1:length(px)  
    for Y = 1:length(py)
      D=[];Q=[];TestFeatures = [px(X) py(Y)];
      for ii=1:nSVs
        D(ii) = norm(SV(ii,:)-[TestFeatures]);
        Q(ii) = exp(-gamma*D(ii).^2);
      end
      pz(Y,X) = Q*w+b;
    end
  end 
  surf(px,py,pz);
  colormap ("jet");
  colorbar ();
  axis([XL XH YL YH]);
  for i=1:size(Classifiers,1) %Classes
    temp = Features{i};
    for ii=1:length(temp(FeatureX,:))
      D=[];Q=[];
      TestFeatures = [temp(FeatureX,ii) temp(FeatureY,ii)];
      for iii=1:nSVs
        D(iii) = norm(SV(iii,:)-[TestFeatures]);
        Q(iii) = exp(-gamma*D(iii).^2);
      end
      Z = Q*w+b;
      if i==ClassifierID
        plot3(temp(FeatureX,ii),temp(FeatureY,ii),Z+0.1,'gd');
      else
        plot3(temp(FeatureX,ii),temp(FeatureY,ii),Z+0.1,'kx');
      end
    end
  end
end