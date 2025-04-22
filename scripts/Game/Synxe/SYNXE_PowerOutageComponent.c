[ComponentEditorProps(category: "Game/", description: "Simulate a power outage within a radius of the owner entity.")]
class SYNXE_PowerOutageComponentClass : ScriptComponentClass {}

class SYNXE_PowerOutageComponent : ScriptComponent {
	[Attribute("POWER_OUTAGE", UIWidgets.Auto, "The name of the mission flag to initiate the power outage.", category: "Settings")]
	protected string m_flag;
	
	[Attribute("500", UIWidgets.Auto, "The radius in meters around the entity to simulate the power outage.", category: "Settings", params: "1 5000")]
	protected int m_radius;
	
	protected int m_baseDelayMs = 50;
	
	override void OnPostInit(IEntity owner) {
		super.OnPostInit(owner);
		
		GetGame().GetCallqueue().CallLater(InitFlagListener, 1000, false);
	}
	
	protected void InitFlagListener() {
		TILW_MissionFrameworkEntity fw = TILW_MissionFrameworkEntity.GetInstance();
		fw.GetOnFlagChanged().Insert(TriggerPowerOutage);
	}
	
	protected void TriggerPowerOutage(string flag, bool value) {
		if (flag == m_flag && value) {
			vector origin = GetOwner().GetOrigin();
			GetOwner().GetWorld().QueryEntitiesBySphere(origin, m_radius, RecursiveStreetLampBreak);
		}
	}
	
	protected bool RecursiveStreetLampBreak(IEntity e) {
		if (!e) return false;
		
		StreetLampComponent light = StreetLampComponent.Cast(e.FindComponent(StreetLampComponent));
		if (light) {
			int jitter = Math.RandomInt(-100, 50);
			int delay = Math.Max(0, m_baseDelayMs + jitter);
			
			GetGame().GetCallqueue().CallLater(DisableLamp, delay, false, light);
			m_baseDelayMs += 25;
		}
		
		IEntity child = e.GetChildren();
		while (child) {
			RecursiveStreetLampBreak(child);
			child = child.GetSibling();
		}
		
		return true;
	}
	
	protected void DisableLamp(StreetLampComponent lamp) {
		lamp.SetBroken(true);
	}
}
