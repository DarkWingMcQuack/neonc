struct TextPosition{
	column: u64,
	row: u64
}

pub struct TextArea <'a>{
	text: &'a str,
	start: TextPosition,
	end: TextPosition
}
