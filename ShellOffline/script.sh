#!/bin/bash

studentCount=$2
totalMarks=$1
initialStudent=1805121

##########################################
##storing parameters and parameter check##
##########################################

if [[ -z "$1" ]]; then
    totalMarks=100
fi
if [[ -z "$2" ]]; then
    studentCount=5
fi

if [[ $studentCount -lt 1 ]] || [[ $totalMarks -lt 1 ]]; then
    echo "invalid parameter value"
    exit
fi

studentIDs=("$initialStudent")
studentNumbers=("$totalMarks")

for ((i = 1; i < $studentCount; i++)); do
    # echo $(($initialStudent + $i))
    studentIDs=(${studentIDs[@]} "$(($initialStudent + $i))")
    studentNumbers=(${studentNumbers[@]} "$totalMarks")
done

##########################################
################markings##################
##########################################

touch buffer.txt
>buffer.txt
for ((i = 0; i < $studentCount; i++)); do

    if ! [[ -e "Submissions/${studentIDs[$i]}/${studentIDs[$i]}.sh" ]]; then
        studentNumbers[$i]=0
        continue
    fi

    bash "Submissions/${studentIDs[$i]}/${studentIDs[$i]}.sh" >buffer.txt

    mistakeCount=0
    for trap in $(diff --ignore-all-space buffer.txt AcceptedOutput.txt); do
        if [[ $trap == ">" ]] || [[ $trap == "<" ]]; then
            mistakeCount=$(($mistakeCount + 1))
        fi
    done
    if [[ ${studentNumbers[$i]} -eq "-100" ]]; then
        studentNumbers[$i]=$((-$totalMarks + 5 * $mistakeCount))
    else
        studentNumbers[$i]=$(($totalMarks - 5 * $mistakeCount))
    fi

    for ((j = $i + 1; j < $studentCount; j++)); do
        if ! [[ -e "Submissions/${studentIDs[$j]}/${studentIDs[$j]}.sh" ]]; then
            continue
        fi
        if [[ -z $(diff --ignore-all-space "Submissions/${studentIDs[$i]}/${studentIDs[$i]}.sh" "Submissions/${studentIDs[$j]}/${studentIDs[$j]}.sh") ]]; then
            if [[ ${studentNumbers[$i]} -gt 0 ]]; then
                studentNumbers[$i]=$((-1 * ${studentNumbers[$i]}))
            fi
            studentNumbers[$j]="-100"
            echo "Copy checker found similar scripts between ${studentIDs[$i]} and ${studentIDs[$j]}"
        fi
    done
done
rm buffer.txt

count=0
echo ""
for student in ${studentIDs[@]}; do
    echo "Student: $student; Marks: ${studentNumbers[$count]}"
    count=$(($count + 1))
done

##########################################
##############csv creator#################
##########################################

touch output.csv
count=0
>output.csv
echo "student_id,score" >>output.csv
for student in ${studentIDs[@]}; do
    echo "$student,${studentNumbers[$count]}" >>output.csv
    count=$(($count + 1))
done
