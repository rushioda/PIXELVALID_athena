
RED='\033[0;31m'
NC='\033[0m'


function usage { 
    [ $# -gt 1 ] && echo $2

    echo "Usage: $(basename $0) [args]"
    echo 
    echo "-d, --directory  DIRECTORY \t run from the specified directory"
    echo "-l, --local                \t run locally rather than on the grid"
    echo "-x, --exclude              \t don't run athena or the post processing, only the plotting stages"
    echo "-p, --post                 \t force running of post processingthe post processing, even if -x is set"
    echo "-f, --force                \t disable protection against rerunning where you shouldn't be"
    echo "-h, --help                 \t this help"
    [ $# -gt 0 ] && exit $1
    exit 0
}

args=$(getopt -ql "searchpath:" -o "d:lxph" -- "$@")

# eval set -- "$args"

RUNATHENA=1
RUNPOST=-1
DIRECTORY=
LOCAL=0
FORCE=0

while [ $# -ge 1 ]; do
    case "$1" in
        --) shift ; break ;;
        -d | --directory )  if [ $# -lt 2 ]; then usage; fi ; DIRECTORY="$2" ; shift ;;
        -x | --exclude )    RUNATHENA=0 ; [ $RUNPOST -eq -1 ] && RUNPOST=0;;
        -p | --post )       RUNPOST=1 ;;
        -f | --force )      FORCE=1 ;;
        -l | --local )      LOCAL=1 ;;
        -h | --help )       usage ;;
     esac
    shift
done


[ $RUNPOST -eq 0 ] || RUNPOST=1


# generate a time stamp

function timestamp { printf "${RED}Timestamp: $1 $(date)${NC}\n"  ; }

# poor quality function to convert seconds to a proper time

function converttime { 
    local totes=$1
    [ $totes -lt 0 ] && ((totes=$totes+86400))
    ((H=$totes/3600))
    ((M=($totes%3600)/60))
    ((S=$totes%60))
    [ $M -lt 10 ] && M=0$M
    [ $S -lt 10 ] && S=0$S
    echo "$H:$M:$S"
}

timestamp "starting"

# reset seconds counter - not really needed, but hey 

SECONDS=0


# stuff to control running concurrent 
# processes 

PROCESS=0
BGPROCS=
NJOBS=8
WAITING=60

# is this needed ? or not ? 
# export ATHENA_NUM_PROC=$NJOBS
# export ATHENA_NUM_PROC=1

unset  ATHENA_NUM_PROC
unset  ATHENA_PROC_NUMBER
 
env | grep ATHENA_NUM_PROC
env | grep ATHENA_PROC_NUMBER




# count how many jobs are running ...

function running {
     local IRUNNING=0
     for BGPROC in $BGPROCS; do ( ps -p $BGPROC &> /dev/null ) && ((IRUNNING++)) ; done
     echo $IRUNNING
}


# wait for an empty slot ...

function waitonproc   {
    echo "waitonproc waiting ..."
    local IRUNNING=$(running)
    while [ $IRUNNING -ge $NJOBS ]; do sleep $WAITING ; IRUNNING=$(running) ; done
}


# wait for all processes to finish ...

function waitonallproc   {
    echo "waitonallproc waiting ..."
    local IRUNNING=$(running)
    while [ $IRUNNING -gt 0 ]; do sleep $WAITING ; IRUNNING=$(running) ; done
}



# run athena  

function runathena { 
   timestamp  "runathena:"

   if [ $# -gt 0 ]; then  

     echo "input $1"

     pwd 

     local DIR=$PWD

     mkdir -p athena-$1
     cd  athena-$1
     cp ../*.py .

     pwd
     echo "ARGS: $ARGS"
     echo -e "\nrunning athena in athena-$1\n"
     athena.py  -c "$ARGS"   @REPLACEJOBOPTIONS  &> athena-local-$1.log
     echo "art-result: $? athena_$1"

     pwd
     ls -lt

     cd $DIR

   fi  

   timestamp  "runathena done:"
}



# function to save the output from the separate 
# athena runs

function saveoutput {
    [ $# -lt 2 ] && echo "not enough arguments for saveoutput" && exit -1  
    mkdir -p $1
    for git in athena-*/$2 ; do
        FILE=$(echo  $git | sed 's|/|-|g')
        cp $git $1/$FILE
    done
}





ls -l



export RTTJOBNAME=@REPLACERTTJOBNAME

jobList=

if [ $LOCAL -eq 1 ]; then
      echo "running locally"
      # get number of files 
      NFILES=$(grep "^#[[:space:]]*art-input-nfiles:" $0 | sed 's|.*art-input-nfiles:[[:space:]]*||g')
      [ $NFILES -lt 1 ] && echo "not enough files: $NFILES" && exit -1
      _jobList=$(TIDAdataset.py $RTTJOBNAME)
      for git in $_jobList ; do [ $NFILES -gt 0 ] || break ; jobList="$jobList ARTConfig=['$git']" ; ((NFILES--)) ; echo "running over $git"  ; done
else
      fileList="['${ArtInFile//,/', '}']"
      _jobList="'../${ArtInFile//,/' '../}'"
      echo "List of files = $fileList"
      for git in $_jobList ; do jobList="$jobList ARTConfig=[$git]" ; echo "ART running over $git"  ; done
fi


if [ $RUNATHENA -eq 1 ]; then 

get_files -jo @REPLACEJOBOPTIONS


# run athena in separate directories


# loop over the inputs spawning the separate athena jobs

i=0

for git in $jobList ; do 

    ARGS="$git;@REPLACECOMMAND"
 
#   echo "ARGS: $ARGS"

    waitonproc
    
    timestamp "forking athena index: $i"

    runathena $i & 
    PID=$!
    BGPROCS="$BGPROCS $PID"

    ((i++))

done


# wait for all the jobs to finish

sleep 10 


PPROCS=$BGPROCS

for PID in $BGPROCS ; do
     for CHILD in $(ps --ppid $PID -o pid h) ; do PPROCS="$PPROCS $CHILD" ; done 
done

[ -e topp.log ] && rm topp.log

echo -e "\nUID        PID  PPID  C    SZ   RSS PSR STIME TTY          TIME CMD" >> topp.log
ps -aF --pid $PPROCS | grep $USER | grep -v grep | grep -v sed | sed 's| [^[:space:]]*/python | python |g' | sed 's| [^[:space:]]*/athena| athena|g' | sed 's|ARTConfig=.* |ARTConfig=... |g' | sed 's|eos/[^[:space:]]*/trigindet|eos/.../trigindet|g' >> topp.log

echo >> topp.log

sleep 20 

top -b -n1 > top.log
grep PID top.log >> topp.log
for PID in $PPROCS ; do grep $PID top.log >> topp.log ; done

cat topp.log

# cat topp.log | mail sutt@cern.ch


echo -e "\n\n"

timestamp "waiting on athena jobs ..."

# echo -e "\n\nwaiting on athena jobs...\n"

waitonallproc

echo "all done ! hooray !"

timestamp "forking all athena jobs completed"


# jobs have all finished, 
# copy the output from the athena run subdirectories to 
# single output data set directories etc

saveoutput output-dataset "Trk*.root"
saveoutput output-logs    "ath*.log"
saveoutput output-cost     trig_cost.root

grep ERROR   athena-*/ath*.log | grep -v ERRORS | grep -v MATRIX | grep -v DERIV &> errors.log 
grep WARNING athena-*/ath*.log &> warnings.log

timestamp "combining expert histograms ..."

# add the expert timing histgrams
hadd expert-monitoring.root athena-*/expert-monitoring.root &> hadd.log 

# this is a hack to get the script to work without needing to fix 
# TIDArun-art.sh which expects TrkNtple-0000.root
# so if we run multiple jobs then we don;t get a Trk*.root 
# file to the check will fail. This creates a link so this 
# test will pass
  
for git in output-dataset/*.root ; do if [ -e $git ]; then ln -s $git TrkNtuple-0000.root ; break ; fi ; done  

[ -e TrkNtuple-0000.root ] || echo "WARNING: all athena stages failed"

fi


ls -lt


