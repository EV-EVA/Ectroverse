#!/bin/bash
 COUNTER=20
 until [  $COUNTER == 10 ]; do
 	./sv > logs/warning$(ls logs/warning* -l | wc -l) 2> logs/error$(ls logs/error* -l | wc -l)
 		echo "Restarted at $(date)"
 		done
 		echo "im done"