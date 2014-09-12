#######################################
SPACE BALL
#######################################
Utilização:

	Para compilar o jogo com o gcc no WINDOWS, utilizar o seguinte comando:
		g++ -o space_ball main.cpp -lmingw32 -lSDL2main -lSDL2

	Para executar o jogo no WINDOWS, utilizar o seguinte comando:
		space_ball maps/<nome_do_mapa>.map

#######################################
Mapas Disponíveis:

	bounce.map:
		Mapa sem gravidade, com bolas que se movem constantemente pelo mapa.
	bounce_planet.map:
		Um único planeta fixo no centro, com bolas que se movem constantemente pelo mapa e são atraídas por este planeta.
	earth.map:
		Um único planeta fixo, simulando a superfície da Terra.
	galaxy.map:
		Diversos planetas fixos em disposição arbitrária.
	moon.map:
		Um planeta fixo no centro com uma bola (um satélite) realizando um movimento circular ao redor.
	planet.map:
		Um único planeta fixo no centro.
	repulsion.map:
		Um grande planeta no topo que repele os jogadores ao invés de atrair.

#######################################
O Jogo:

	O jogo requer exatos 2 jogadores.
	Cada jogador controla uma nave, competindo contra o outro jogador.
	O objetivo do jogo é atirar no jogador inimigo até vencer.
	A barra no topo indica o progesso até o fim do jogo.
	A velocidade das naves são afetadas pelos planetas, que atraem gravitacionalmente.
	Os tiros também são afetados pela gravidade.

	Os tiros desaparecem ao colidir com objetos no mapa pela segunda vez.
	Na primeira colisão do tiro, o tiro reflete. Na colisão do tiro com as bordas do mapa, o tiro é destruído.
	Quando um tiro atinge um jogador, a barra no topo avança no sentido do jogador inimigo. Neste caso, o tiro é destruído imediatamente.
	Quando um jogador colide com um objeto, ele possui sua trajetória refletida.
	Quando um jogador colide com a borda do mapa, ele possui sua trajetória refletida.

#######################################
Controles:

	Jogador 1
		UP: Acelera para frente.
		DOWN: Acelera para trás.
		RIGHT: Rotaciona no sentido horário.
		LEFT: Rotaciona no sentido anti-horário.
		SPACE: Atira para frente.

	Jogador 1
		MOUSE: Rotaciona/Mira a nave.
		MOUSE_RIGHT_BUTTON: Acelera para frente.
		MOUSE_LEFT_BUTTON: Atira para frente.

#######################################