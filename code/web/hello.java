import java.awt.Font;
import java.awt.Graphics;
import java.applet.Applet;
public class hello extends Applet
{
	public String sHello;
	public void init(){
		sHello="Hello World!";
	}
	public void paint(Graphics g){
		g.setFont(new Font("Tahoma", Font.BOLD, 20));
		g.drawString(sHello,100,100);
	}
}