use bracket_lib::prelude::*;

enum GameMode {
    Menu,
    Playing,
    End,
}

const SCREEN_WIDTH: i32 = 80;
const SCREEN_HEIGHT: i32 = 50;
const FRAME_DURATION: f32 = 75.0;

enum Direction {
    Right,
    Down,
    Left,
    Up,
}

impl Direction {
    fn is_opposite(&self, other: &Direction) -> bool {
        match (self, other) {
            (Direction::Right, Direction::Left) => true,
            (Direction::Left, Direction::Right) => true,
            (Direction::Down, Direction::Up) => true,
            (Direction::Up, Direction::Down) => true,
            _ => false,
        }
    }
}

struct Player {
    x: i32,
    y: i32,
    dir: Direction,
    head: char,
    body: Vec<(i32, i32)>,
    grow: bool,
}

impl Player {
    fn new(x: i32, y: i32, dir: Direction) -> Self {
        Player {
            x,
            y,
            dir: dir,
            head: '█',
            body: vec![],
            grow: false,
        }
    }

    fn render(&mut self, ctx: &mut BTerm) {
        ctx.set(self.x, self.y, YELLOW, BLACK, to_cp437(self.head));
        for &(x, y) in &self.body {
            ctx.set(x, y, YELLOW, BLACK, to_cp437(self.head));
        }
    }

    fn move_player(&mut self) {
        let old_head = (self.x, self.y);
        match self.dir {
            Direction::Right => self.x += 1,
            Direction::Left => self.x -= 1,
            Direction::Down => self.y += 1,
            Direction::Up => self.y -= 1,
        }
        if self.grow {
            self.body.push(old_head);
            self.grow = false;
        }
        if !self.body.is_empty() {
            for i in (1..self.body.len()).rev() {
                self.body[i] = self.body[i - 1];
            }
            self.body[0] = old_head;
        }
    }

    fn grow(&mut self) {
        self.grow = true;
    }

    fn check_collision(&self) -> bool {
        for &(body_x, body_y) in &self.body {
            if self.x == body_x && self.y == body_y {
                return true;
            }
        }
        false
    }
}

struct Obstacle {
    x1: i32,
    x2: i32,
    y1: i32,
    y2: i32,
}

impl Obstacle {
    fn new() -> Self {
        Obstacle {
            x1: 2,
            x2: SCREEN_WIDTH - 3,
            y1: 2,
            y2: SCREEN_HEIGHT - 3,
        }
    }

    fn render(&mut self, ctx: &mut BTerm) {
        // Draw left and right vertical lines
        for y in self.y1..=self.y2 {
            ctx.set(self.x1, y, RED, BLACK, to_cp437('|'));
            ctx.set(self.x2, y, RED, BLACK, to_cp437('|'));
        }

        // Draw top and bottom horizontal lines
        for x in self.x1..=self.x2 {
            ctx.set(x, self.y1, RED, BLACK, to_cp437('-'));
            ctx.set(x, self.y2, RED, BLACK, to_cp437('-'));
        }
    }
    fn hit_obstacle(&self, player: &Player) -> bool {
        player.x <= self.x1 || player.x >= self.x2 || player.y <= self.y1 || player.y >= self.y2
    }
}

struct Food {
    x: i32,
    y: i32,
}
impl Food {
    fn new() -> Self {
        let mut random_x = RandomNumberGenerator::new();
        let mut random_y = RandomNumberGenerator::new();
        Food {
            x: random_x.range(3, SCREEN_WIDTH - 4),
            y: random_y.range(3, SCREEN_HEIGHT - 4),
        }
    }

    fn render(&mut self, ctx: &mut BTerm) {
        ctx.set(self.x, self.y, YELLOW, BLACK, to_cp437('█'));
    }
}
struct State {
    player: Player,  // add the player to the game
    frame_time: f32, //variable to control game speed
    obstacle: Obstacle,
    food: Food,
    mode: GameMode,
    score: i32,
}

impl State {
    fn new() -> Self {
        State {
            player: Player::new(5, 25, Direction::Right),
            frame_time: 0.0,
            obstacle: Obstacle::new(),
            food: Food::new(),
            mode: GameMode::Menu,
            score: 0,
        }
    }

    fn play(&mut self, ctx: &mut BTerm) {
        ctx.cls_bg(NAVY);
        ctx.print(0, 0, "Use WASD to move");
        ctx.print(0, 1, &format!("Score: {}", self.score));
        self.obstacle.render(ctx);
        self.food.render(ctx);
        self.frame_time += ctx.frame_time_ms;
        if self.frame_time > FRAME_DURATION {
            self.frame_time = 0.0;
            self.player.move_player();
        }
        if let Some(key) = ctx.key {
            let new_dir = match key {
                VirtualKeyCode::A => Direction::Left,
                VirtualKeyCode::D => Direction::Right,
                VirtualKeyCode::W => Direction::Up,
                VirtualKeyCode::S => Direction::Down,
                _ => return, // Ignore other keys
            };

            if !self.player.dir.is_opposite(&new_dir) {
                self.player.dir = new_dir;
            }
        }
        self.player.render(ctx);
        if self.obstacle.hit_obstacle(&self.player) {
            self.mode = GameMode::End;
        }
        if self.player.check_collision() {
            self.mode = GameMode::End;
        }
        if self.player.x == self.food.x && self.player.y == self.food.y {
            self.player.grow();
            self.score += 1;
            self.food = Food::new();
        }
    }

    fn restart(&mut self) {
        self.player = Player::new(5, 25, Direction::Right);
        self.frame_time = 0.0;
        self.obstacle = Obstacle::new();
        self.food = Food::new();
        self.mode = GameMode::Playing;
        self.score = 0;
    }

    fn main_menu(&mut self, ctx: &mut BTerm) {
        ctx.cls();
        ctx.print_centered(5, "Welcome");
        ctx.print_centered(8, "(P) Play game");
        ctx.print_centered(9, "(Q) Quite Game");
        if let Some(key) = ctx.key {
            match key {
                VirtualKeyCode::P => self.restart(),
                VirtualKeyCode::Q => ctx.quitting = true,
                _ => {}
            }
        }
    }

    fn dead(&mut self, ctx: &mut BTerm) {
        ctx.cls();
        ctx.print_centered(5, "You are dead");
        ctx.print_centered(6, &format!("You earned {} points", self.score));
        ctx.print_centered(8, "(P) Play game");
        ctx.print_centered(9, "(Q) Quite Game");
        if let Some(key) = ctx.key {
            match key {
                VirtualKeyCode::P => self.restart(),
                VirtualKeyCode::Q => ctx.quitting = true,
                _ => {}
            }
        }
    }
}

impl GameState for State {
    fn tick(&mut self, ctx: &mut BTerm) {
        match self.mode {
            GameMode::Menu => self.main_menu(ctx),
            GameMode::End => self.dead(ctx),
            GameMode::Playing => self.play(ctx),
        }
    }
}

fn main() -> BError {
    let context = BTermBuilder::simple80x50().with_title("Snake").build()?;
    main_loop(context, State::new())
}
