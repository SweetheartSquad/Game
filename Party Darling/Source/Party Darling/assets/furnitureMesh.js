{
	furniture:
	[
		{
			"type":"chair",
			"components":
				[
					{
						"componentType":"seat",
						"required":"true"
						"outComponents":
						[
							{
								"childComponent":"back",
								"required":"true",
								"multiplier":"1"
								"outComponents":
								[
									{
									"childComponent":"arm",
									"required":"false",
									"multiplier":"2",
									}
								]
							},
							{
								"childComponent":"leg",
								"required":"true",
								"multiplier":"4"
								"outComponents":
								[
									{
									"childComponent":"legSupport",
									"required":"false",
									"multiplier":"2",
									}
								]
							},
						
						]
					}
					
				]
		}
	
	],
	components
	[
		{
			"id":"1",
			"type:"seat",
			"src":"chair_Seat_1.obj"
			"connectors":
			[
				{
					"description":"back",
					"position":"0,0.29,-3.416"
				},
				{
					"description":"front_left_leg",
					"position":"3.24,-0.29,3.416"
				},
				{
					"description":"front_right_leg",
					"position":"-3.24,-0.29,3.416"
				},
				{
					"description":"back_left_leg",
					"position":"3.24,-0.29,-3.416"
				},
				{
					"description":"back_right_leg",
					"position":"-3.24,-0.29,-3.416"
				}
			]
		},
	
		{
			"id":"2",
			"type:"back",
			"src":"chair_Back_1.obj"
			"connectors":
			[
				{
					"description":"rightSide",
					"position":"-3.573"
				},
				{
					"description":"leftSide",
					"position":"3.573"
				},
				{
					"description":"height",
					"position":"13.231"
				}
			]
		},
	
	]


}