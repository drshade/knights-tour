object Problem {
	type Board = Array[Array[Boolean]]
	val boardsize = 5
	val startingPositions = for { x <- (0 until boardsize) ; y <- (0 until boardsize) } yield (x,y)
	def legalmoves(x: Int, y: Int) = {
		val possibles = List((x-1, y-2), (x-2,y-1), (x+1, y-2), (x+2, y-1), (x+1, y+2), (x+2, y+1), (x-1, y+2), (x-2, y+1))
		possibles.filter(a => a._1 >= 0 && a._2 >=0 && a._1 < boardsize && a._2 < boardsize)
	}

	def clone(x: Board) = { // Thanks Java :(
		val y = Array.fill(boardsize, boardsize)(true)
		for { i <- (0 until boardsize)
		      j <- (0 until boardsize) }
		      y(i)(j) = x(i)(j)
		y
	}

	def availablemove(x: Int, y: Int, b: Board) = b(x)(y)
	def hunt(startx: Int, starty: Int, b: Board)(hist: List[(Int, Int)])(f: List[(Int, Int)] => Unit): Unit = {
		b(startx)(starty) = false
		if (hist.length == boardsize * boardsize) {
			print(".")
			f(hist)
			} else {
			val availablemoves = legalmoves(startx, starty).filter(a => availablemove(a._1, a._2, b))
			availablemoves.par.foreach(a => hunt(a._1, a._2, clone(b))((a._1,a._2) :: hist)(f))
		}
	}

	def main(args: Array[String]) {
		var solutions = List[List[(Int, Int)]]()
		def addSolution(a: List[(Int, Int)]) = { solutions = a :: solutions }
		startingPositions.par.foreach(pos => hunt(pos._1, pos._2, Array.fill(boardsize, boardsize)(true))(List((pos._1, pos._2)))(addSolution))

		println("Solutions: " + solutions.length)
	}
}