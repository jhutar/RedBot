case $(($((RANDOM))%6)) in
	0)
		echo "v"
		;;
	1)
		echo "r"
		;;
	2)
		echo "z"
		;;
	3)
		echo "f"
		;;
	4)
		echo "s"
		;;
	5)
		echo "u $(($(($((RANDOM))%4))+1)) 1"
		;;
esac
